#include <iostream>
#include <Utilities/Timer.h>
#include <Utilities/Utilities_print.h>
#include <cstring>
#include "ConvexMPCLocomotion.h"
#include "convexMPC_interface.h"
#include "../../../../common/FootstepPlanner/GraphSearch.h"

#include "Gait.h"
#include <string>
// Todo: maxforce(48,630), iteration_between_mpc(46), kp kd of cartesian when locomotion(321-331)
// Todo: mpc parameters Q[12], alpha(598,603,722,727) , robot parameters(709-714)

//#define DRAW_DEBUG_SWINGS
//#define DRAW_DEBUG_PATH
//#define ABANDONED

////////////////////
// Controller
////////////////////

ConvexMPCLocomotion::ConvexMPCLocomotion(float _dt, int _iterations_between_mpc, MiLAB_UserParameters* parameters, float fmax, RobotType &robotType) :
        iterationsBetweenMPC(_iterations_between_mpc),
        horizonLength(10),
        dt(_dt),
        trotting(horizonLength, Vec4<int>(0,5,5,0), Vec4<int>(5,5,5,5),"Trotting"),
        bounding(horizonLength, Vec4<int>(5,5,0,0),Vec4<int>(4,4,4,4),"Bounding"),
        //bounding(horizonLength, Vec4<int>(5,5,0,0),Vec4<int>(3,3,3,3),"Bounding"),
        pronking(horizonLength, Vec4<int>(0,0,0,0),Vec4<int>(4,4,4,4),"Pronking"),
        jumping(horizonLength, Vec4<int>(0,0,0,0), Vec4<int>(2,2,2,2), "Jumping"),
        //galloping(horizonLength, Vec4<int>(0,2,7,9),Vec4<int>(6,6,6,6),"Galloping"),
        //galloping(horizonLength, Vec4<int>(0,2,7,9),Vec4<int>(3,3,3,3),"Galloping"),
        galloping(horizonLength, Vec4<int>(0,2,7,9),Vec4<int>(4,4,4,4),"Galloping"),
        standing(horizonLength, Vec4<int>(0,0,0,0),Vec4<int>(10,10,10,10),"Standing"),
        //trotRunning(horizonLength, Vec4<int>(0,5,5,0),Vec4<int>(3,3,3,3),"Trot Running"),
        trotRunning(horizonLength, Vec4<int>(0,5,5,0),Vec4<int>(4,4,4,4),"Trot Running"),
        walking(horizonLength, Vec4<int>(0,3,5,8), Vec4<int>(5,5,5,5), "Walking"),
        walking2(horizonLength, Vec4<int>(0,5,5,0), Vec4<int>(7,7,7,7), "Walking2"),
        pacing(horizonLength, Vec4<int>(5,0,5,0),Vec4<int>(5,5,5,5),"Pacing"),
        random(horizonLength, Vec4<int>(9,13,13,9), 0.4, "Flying nine thirteenths trot"),
        random2(horizonLength, Vec4<int>(8,16,16,8), 0.5, "Double Trot")
{
    _parameters = parameters;
    dtMPC = dt * iterationsBetweenMPC;
    default_iterations_between_mpc = iterationsBetweenMPC;
    printf("[Convex MPC] dt: %.3f iterations: %d, dtMPC: %.4f, maxforce: %.1f\n", dt, iterationsBetweenMPC, dtMPC,fmax);
    setup_problem(dtMPC, horizonLength, 0.4, fmax);
    rpy_comp[0] = 0;
    rpy_comp[1] = 0;
    rpy_comp[2] = 0;
    rpy_int[0] = 0;
    rpy_int[1] = 0;
    rpy_int[2] = 0;

    for(int i = 0; i < 4; i++)
        firstSwing[i] = true;

    if (robotType == RobotType::IUST){
        initMilabSparseMPC();
    }else{
        initSparseMPC();
    }

    pBody_des.setZero();
    vBody_des.setZero();
    aBody_des.setZero();
}

void ConvexMPCLocomotion::initialize(){
    for(int i = 0; i < 4; i++) firstSwing[i] = true;
    firstRun = true;
#ifdef ABANDONED
    std::ofstream outf_data("/home/allen/MiLAB-Cheetah-Software/debug_tools/mpc_control_data.txt", std::fstream::out | std::ios_base::trunc);
#endif
}

void ConvexMPCLocomotion::recompute_timing(int iterations_per_mpc) {
    iterationsBetweenMPC = iterations_per_mpc;
    dtMPC = dt * iterations_per_mpc;
}

void ConvexMPCLocomotion::_SetupCommand(ControlFSMData<float> & data){
    if(data._quadruped->_robotType == RobotType::IUST) {
        _body_height = 0.375;
    }else if(data._quadruped->_robotType == RobotType::MINI_CHEETAH){
        _body_height = 0.29;
    }else if(data._quadruped->_robotType == RobotType::CHEETAH_3){
        _body_height = 0.45;
    }else{
        assert(false);
    }

    float x_vel_cmd, y_vel_cmd;
    float filter(0.1);
    if(data.controlParameters->use_rc){ // remote commander
        const rc_control_settings* rc_cmd = data._desiredStateCommand->rcCommand;
        data.userParameters->cmpc_gait = rc_cmd->variable[0];
        _yaw_turn_rate = -rc_cmd->omega_des[2];
        x_vel_cmd = rc_cmd->v_des[0] * 0.5;
        y_vel_cmd = rc_cmd->v_des[1] * 0.25;
        _body_height += rc_cmd->height_variation * 0.05;
        step_height = rc_cmd->step_height*0.1;
    }else{ //simulation gamepad
        _yaw_turn_rate = data._desiredStateCommand->rightAnalogStick[0]*1.0;
        x_vel_cmd = data._desiredStateCommand->leftAnalogStick[1]*1.0;
        y_vel_cmd = data._desiredStateCommand->leftAnalogStick[0]*0.5;
        step_height = data.userParameters->Swing_traj_height;
    }
    _x_vel_des = _x_vel_des*(1-filter) + x_vel_cmd * filter;
    _y_vel_des = _y_vel_des*(1-filter) + y_vel_cmd * filter;

    _yaw_des = data._stateEstimator->getResult().rpy[2] + dt * _yaw_turn_rate;
    _roll_des = 0.;
    _pitch_des = 0.;

}
#ifdef ABANDONED
void ConvexMPCLocomotion::outpacket(ControlFSMData<float>& data){

    std::ofstream outf_data;
    outf_data.open("/home/allen/MiLAB-Cheetah-Software/debug_tools/mpc_control_data.txt", std::ios::app);
    auto& state = data._stateEstimator->getResult();
    if (!outf_data.is_open()) {
        std::cout << "[ConvexMPC] Open mpc_control_data.txt failed!" << std::endl;
    } else {
//   //     LegControllerData & LegControllerCommand -36
        for (int leg = 0; leg < 4; leg++) {
            outf_data << " " << data._legController->datas[leg].q[0] << " " <<  data._legController->datas[leg].q[1] << " " <<  data._legController->datas[leg].q[2] << " ";
            outf_data << " " << data._legController->datas[leg].qd[0] << " " <<  data._legController->datas[leg].qd[1] << " " <<  data._legController->datas[leg].qd[2] << " ";
            outf_data << data._legController->datas[leg].tauEstimate[0] << " " << data._legController->datas[leg].tauEstimate[1] << " "
                      << data._legController->datas[leg].tauEstimate[2] << " ";
        }


//   //  StateEstimate -37
        outf_data << state.position[0] << " " << state.position[1] << " " << state.position[2] << " "
                  << state.vBody[0]<< " " << state.vBody[1]<< " " << state.vBody[2]<< " "
                  << state.orientation[0] << " " << state.orientation[1] << " " << state.orientation[2] << " " << state.orientation[3] << " "
                  << state.omegaBody[0] << " " << state.omegaBody[1] << " " << state.omegaBody[2] << " "
                  << state.rBody(0,0) << " " << state.rBody(0,1) << " " << state.rBody(0,2) << " "
                  << state.rBody(1,0) << " " << state.rBody(1,1) << " " << state.rBody(1,2) << " "
                  << state.rBody(2,0) << " " << state.rBody(2,1) << " " << state.rBody(2,2) << " "
                  << state.rpy[0] << " " << state.rpy[1] << " " << state.rpy[2] << " "
                  << state.omegaWorld[0] << " " << state.omegaWorld[1] << " " << state.omegaWorld[2] << " "
                  << state.vWorld[0] << " " << state.vWorld[1] << " " << state.vWorld[2] << " "
                  << state.aBody[0] << " " <<  state.aBody[1] << " " << state.aBody[2] << " "
                  << state.aWorld[0] << " " << state.aWorld[1] << " " << state.aWorld[2] << " ";
                  outf_data << std::endl;

        outf_data.close();
    }
}
//"/home/allen/MiLAB-Cheetah-Software/debug_tools/mpc_control_data.txt"
void ConvexMPCLocomotion::readpacket(int iter,StateEstimate<float> state,LegController<float> *data){
    std::ifstream inf_data("/home/robot/robot-software/build/mpc_control_data.txt", std::ios::in);
    std::string line_info,input_result;
    vector<std::string> vectorString;
    if(inf_data)
    {

        for (int i = 0; i <= iter; ++i) {
            getline (inf_data, line_info);
        }

        std::stringstream input(line_info);
        while(input>>input_result)
            vectorString.push_back(input_result);

        Vec3<float> position; position[2] = atof(vectorString[38].c_str());
        printf("body height %f \n",position[2]);

        for (int i = 0; i < (int)vectorString.size(); ++i) {
            if (data->datas[0].q[0]==0){}
            if (i == 0)
                for (int j = 0; j < 4; ++j) {

                        for (int l = 0; l < 3; ++l) {
                            data->datas[j].q[l] = atof(vectorString[9*j+l].c_str());
                            data->datas[j].qd[l] = atof(vectorString[9*j+3*1+l].c_str());
//                            data->datas[j].tauEstimate[l] = atof(vectorString[9*j+3*2+l].c_str());
                        }


                }
            if (i == 36)
                for (int j = 0; j < 3; ++j) {
                    state.position[j] = atof(vectorString[36+j].c_str());
                }
            else if (i == 39)
                for (int j = 0; j < 3; ++j) {
                    state.vBody[j] = atof(vectorString[39+j].c_str());
                }
            else if (i == 42)
                foseResult.position[0];
        stand_traj[1] = seResult.position[1];r (int j = 0; j < 4; ++j) {
                    state.orientation[j] = atof(vectorString[42+j].c_str());
                }
            else if (i == 46)
                for (int j = 0; j < 3; ++j) {
                    state.omegaBody[j] = atof(vectorString[46+j].c_str());
                }
            else if (i == 49)
                for (int j = 0; j < 3; ++j) {
                    for (int k = 0; k < 3; ++k) {
                        state.rBody(j,k) = atof(vectorString[48+3*j+k].c_str());
                    }
                }
            else if (i == 58)
                for (int j = 0; j < 3; ++j) {
                    state.rpy[j] = atof(vectorString[58+j].c_str());
                }
            else if (i == 61)
                for (int j = 0; j < 3; ++j) {
                    state.omegaWorld[j] = atof(vectorString[61+j].c_str());
                }
            else if (i == 64)
                for (int j = 0; j < 3; ++j) {
                    state.vWorld[j] = atof(vectorString[64+j].c_str());
                }
            else if (i == 67)
                for (int j = 0; j < 3; ++j) {
                    state.aBody[j] = atof(vectorString[67+j].c_str());
                }
            else if (i == 70)
                for (int j = 0; j < 3; ++j) {
                    state.aWorld[j] = atof(vectorString[70+j].c_str());
                }
        }



    }else {
        std::cout << "[ConvexMPC] Read mpc_control_data.txt failed!" << std::endl;
    }
}
#endif
template<>
void ConvexMPCLocomotion::run(ControlFSMData<float>& data) {
    bool omniMode = false;
#ifdef ABANDONED
    outpacket(data);
#endif

    // Command Setup
    _SetupCommand(data);
    gaitNumber = data.userParameters->cmpc_gait;
    if(gaitNumber >= 10) {
        gaitNumber -= 10;
        omniMode = true;
    }
    auto& seResult = data._stateEstimator->getResult();

#ifdef ABANDONED
    readpacket(iterationCounter,seResult,data._legController);
#endif
    // Check if transition to standing
    if(((gaitNumber == 4) && current_gait != 4) || firstRun)
    {
        stand_traj[0] = seResult.position[0];
        stand_traj[1] = seResult.position[1];
        if(data._quadruped->_robotType != RobotType::MINI_CHEETAH) {
            stand_traj[2] = 0.30;
        }else if(data._quadruped->_robotType == RobotType::MINI_CHEETAH){
            stand_traj[2] = 0.21;}
        stand_traj[3] = 0;
        stand_traj[4] = 0;
        stand_traj[5] = seResult.rpy[2];
        world_position_desired[0] = stand_traj[0];
        world_position_desired[1] = stand_traj[1];
    }

    // pick gait
    Gait* gait = &trotting;
    if(gaitNumber == 1)
        gait = &bounding;
    else if(gaitNumber == 2)
        gait = &pronking;
    else if(gaitNumber == 3)
        gait = &walking;
    else if(gaitNumber == 4)
        gait = &standing;
    else if(gaitNumber == 5)
        gait = &trotRunning;
    else if(gaitNumber == 6)
        gait = &galloping;
    else if(gaitNumber == 7)
        gait = &random2;
    else if(gaitNumber == 8)
        gait = &pacing;
    current_gait = gaitNumber;

    gait->setIterations(iterationsBetweenMPC, iterationCounter);
    recompute_timing(default_iterations_between_mpc);

    if (data._quadruped->_robotType == RobotType::IUST) {
        _body_height = 0.35;
    } else if (data._quadruped->_robotType == RobotType::MINI_CHEETAH) {
        _body_height = 0.29;
    } else if (data._quadruped->_robotType == RobotType::CHEETAH_3) {
        _body_height = 0.45;
    }
    // integrate position setpoint
    Vec3<float> v_des_robot(_x_vel_des, _y_vel_des, 0);
    Vec3<float> v_des_world =
            omniMode ? v_des_robot : seResult.rBody.transpose() * v_des_robot;
    Vec3<float> v_robot = seResult.vWorld;

    //pretty_print(v_des_world, std::cout, "v des world");

    //Integral-esque pitche and roll compensation
    if(fabs(v_robot[0]) > .02)   //avoid dividing by zero
    {
        rpy_int[1] += 5*dt*(_pitch_des - seResult.rpy[1])/v_robot[0];
    }
    if(fabs(v_robot[1]) > 0.01)
    {
        rpy_int[0] += dt*(_roll_des - seResult.rpy[0])/v_robot[1];
    }

    rpy_int[0] = fminf(fmaxf(rpy_int[0], -.25), .25);
    rpy_int[1] = fminf(fmaxf(rpy_int[1], -.25), .25);
    rpy_comp[1] = v_robot[0] * rpy_int[1];
    rpy_comp[0] = v_robot[1] * rpy_int[0] * (gaitNumber!=2);  //turn off for pronking

//    foot postion in world frame
    for(int i = 0; i < 4; i++) {
        pFoot[i] = seResult.position + seResult.rBody.transpose() * (data._quadruped->getHipLocation(i)
                   + data._legController->datas[i].p);
    }

    if(gait != &standing) {
        world_position_desired += dt * Vec3<float>(v_des_world[0], v_des_world[1], 0);

    }

    // some first time initialization
    if(firstRun)
    {
        world_position_desired[0] = seResult.position[0];
        world_position_desired[1] = seResult.position[1];
        world_position_desired[2] = seResult.rpy[2];

        for(int i = 0; i < 4; i++)
        {

            footSwingTrajectories[i].setHeight(step_height);
            footSwingTrajectories[i].setInitialPosition(pFoot[i]);
            footSwingTrajectories[i].setFinalPosition(pFoot[i]);

        }
        firstRun = false;
    }

    // foot placement
    for(int l = 0; l < 4; l++)
        swingTimes[l] = gait->getCurrentSwingTime(dtMPC, l);

    float side_sign[4] = {-1, 1, -1, 1};
    float interleave_y[4] = {0.0, 0.0, 0.02, -0.02};//{-0.08, 0.08, 0.02, -0.02};
    //float interleave_gain = -0.13;
    float interleave_gain = 0.;
    //float v_abs = std::fabs(seResult.vBody[0]);
    float v_abs = std::fabs(v_des_robot[0]);
    for(int i = 0; i < 4; i++)
    {

        if(firstSwing[i]) {
            swingTimeRemaining[i] = swingTimes[i];
        } else {
            swingTimeRemaining[i] -= dt;
        }

        footSwingTrajectories[i].setHeight(step_height);

        Vec3<float> offset;
        if (data._quadruped->_robotType == RobotType::IUST){
            offset << 0, side_sign[i] * 0.125, 0;
        } else{
            offset << 0, side_sign[i] * .065, 0;
        }
        if(i<2)
            offset[0]=0;
        else
            offset[0]=-0.02;
    //       get real hip location from com
        Vec3<float> pRobotFrame = (data._quadruped->getHipLocation(i) + offset);

        pRobotFrame[1] += interleave_y[i] * v_abs * interleave_gain;
        float stance_time = gait->getCurrentStanceTime(dtMPC, i);
        Vec3<float> pYawCorrected =
                coordinateRotation(CoordinateAxis::Z, -_yaw_turn_rate* stance_time / 2) * pRobotFrame;


        Vec3<float> des_vel;
        des_vel[0] = _x_vel_des;
        des_vel[1] = _y_vel_des;
        des_vel[2] = 0.0;

        Vec3<float> Pf = seResult.position +
                         seResult.rBody.transpose() * (pYawCorrected + des_vel * swingTimeRemaining[i]);

        //float p_rel_max = 0.35f;
        float p_rel_max = 0.3f;

        // Using the estimated velocity is correct
        //Vec3<float> des_vel_world = seResult.rBody.transpose() * des_vel;
        float pfx_rel = seResult.vWorld[0] * (.5 + _parameters->cmpc_bonus_swing) * stance_time +
                        .1f*(seResult.vWorld[0]-v_des_world[0]) +
                        (0.5f*seResult.position[2]/9.81f) * (seResult.vWorld[1]*_yaw_turn_rate);

        float pfy_rel = seResult.vWorld[1] * .5 * stance_time * dtMPC +
                        .1f*(seResult.vWorld[1]-v_des_world[1]) +
                        (0.5f*seResult.position[2]/9.81f) * (-seResult.vWorld[0]*_yaw_turn_rate);
        pfx_rel = fminf(fmaxf(pfx_rel, -p_rel_max), p_rel_max);
        pfy_rel = fminf(fmaxf(pfy_rel, -p_rel_max), p_rel_max);
        Pf[0] +=  pfx_rel;
        Pf[1] +=  pfy_rel;
        Pf[2] = -0.01;

        footSwingTrajectories[i].setFinalPosition(Pf);
    }
    // calc gait
    iterationCounter++;

    Kp_swing = data.userParameters->Swing_Kp_cartesian;
    Kd_swing = data.userParameters->Swing_Kd_cartesian;
    // load LCM leg swing gains
    Kp << Kp_swing[0], 0, 0,
          0, Kp_swing[1], 0,
          0, 0, Kp_swing[2];
    Kp_stance = 0*Kp;

    Kd << Kd_swing[0], 0, 0,
          0, Kd_swing[1], 0,
          0, 0, Kd_swing[2];
    Kd_stance = Kd;
    // gait
    Vec4<float> contactStates = gait->getContactState();
    Vec4<float> swingStates = gait->getSwingState();
    int* mpcTable = gait->getMpcTable();
    // compute ff_f by MPC solver
    updateMPCIfNeeded(mpcTable, data, omniMode);

    //  StateEstimator* se = hw_i->state_estimator;
    Vec4<float> se_contactState(0,0,0,0);

#ifdef DRAW_DEBUG_PATH
    auto* trajectoryDebug = data.visualizationData->addPath();
  if(trajectoryDebug) {
    trajectoryDebug->num_points = 10;
    trajectoryDebug->color = {0.2, 0.2, 0.7, 0.5};
    for(int i = 0; i < 10; i++) {
      trajectoryDebug->position[i][0] = trajAll[12*i + 3];
      trajectoryDebug->position[i][1] = trajAll[12*i + 4];
      trajectoryDebug->position[i][2] = trajAll[12*i + 5];
      auto* ball = data.visualizationData->addSphere();
      ball->radius = 0.01;
      ball->position = trajectoryDebug->position[i];
      ball->color = {1.0, 0.2, 0.2, 0.5};
    }
  }
#endif

    for(int foot = 0; foot < 4; foot++)
    {
        float contactState = contactStates[foot];
        float swingState = swingStates[foot];
        if(swingState > 0) // foot is in swing
        {
            if(firstSwing[foot])
            {
                firstSwing[foot] = false;
                footSwingTrajectories[foot].setInitialPosition(pFoot[foot]);
            }

#ifdef DRAW_DEBUG_SWINGS
            auto* debugPath = data.visualizationData->addPath();
      if(debugPath) {
        debugPath->num_points = 100;
        debugPath->color = {0.2,1,0.2,0.5};
        float step = (1.f - swingState) / 100.f;
        for(int i = 0; i < 100; i++) {
          footSwingTrajectories[foot].computeSwingTrajectoryBezier(swingState + i * step, swingTimes[foot]);
          debugPath->position[i] = footSwingTrajectories[foot].getPosition();
        }
      }
      auto* finalSphere = data.visualizationData->addSphere();
      if(finalSphere) {
        finalSphere->position = footSwingTrajectories[foot].getPosition();
        finalSphere->radius = 0.02;
        finalSphere->color = {0.6, 0.6, 0.2, 0.7};
      }
      footSwingTrajectories[foot].computeSwingTrajectoryBezier(swingState, swingTimes[foot]);
      auto* actualSphere = data.visualizationData->addSphere();
      auto* goalSphere = data.visualizationData->addSphere();
      goalSphere->position = footSwingTrajectories[foot].getPosition();
      actualSphere->position = pFoot[foot];
      goalSphere->radius = 0.02;
      actualSphere->radius = 0.02;
      goalSphere->color = {0.2, 1, 0.2, 0.7};
      actualSphere->color = {0.8, 0.2, 0.2, 0.7};
#endif
            footSwingTrajectories[foot].computeSwingTrajectoryBezier(swingState, swingTimes[foot]);

            Vec3<float> pDesFootWorld = footSwingTrajectories[foot].getPosition();
            Vec3<float> vDesFootWorld = footSwingTrajectories[foot].getVelocity();
            Vec3<float> pDesLeg = seResult.rBody * (pDesFootWorld - seResult.position)
                    - data._quadruped->getHipLocation(foot);
            Vec3<float> vDesLeg = seResult.rBody * (vDesFootWorld - seResult.vWorld);

            // Update for WBC
            pFoot_des[foot] = pDesFootWorld;
            vFoot_des[foot] = vDesFootWorld;
            aFoot_des[foot] = footSwingTrajectories[foot].getAcceleration();
            //only using MPC
            if(!data.userParameters->use_wbc){
                // Update leg control command regardless of the usage of WBIC
                data._legController->commands[foot].pDes = pDesLeg;
                data._legController->commands[foot].vDes = vDesLeg;
                data._legController->commands[foot].kpCartesian = Kp;
                data._legController->commands[foot].kdCartesian = Kd;
            }
        }
        else // foot is in stance
        {
            firstSwing[foot] = true;

#ifdef DRAW_DEBUG_SWINGS
            auto* actualSphere = data.visualizationData->addSphere();
      actualSphere->position = pFoot[foot];
      actualSphere->radius = 0.02;
      actualSphere->color = {0.2, 0.2, 0.8, 0.7};
#endif

            Vec3<float> pDesFootWorld = footSwingTrajectories[foot].getPosition();
            Vec3<float> vDesFootWorld = footSwingTrajectories[foot].getVelocity();
            Vec3<float> pDesLeg = seResult.rBody * (pDesFootWorld - seResult.position) - data._quadruped->getHipLocation(foot);
            Vec3<float> vDesLeg = seResult.rBody * (vDesFootWorld - seResult.vWorld);
//            std::cout << "Foot" << foot << " relative foot position: " << pDesFootWorld.transpose() <<"body position"<<seResult.position.transpose()<<" hip pos:"<<data._quadruped->getHipLocation(foot)<< "\n";

            //  only using MPC
            if(!data.userParameters->use_wbc){
                data._legController->commands[foot].pDes = pDesLeg;
                data._legController->commands[foot].vDes = vDesLeg;
                data._legController->commands[foot].kpCartesian = 0*Kp_stance;
                data._legController->commands[foot].kdCartesian = Kd_stance;

                data._legController->commands[foot].forceFeedForward = f_ff[foot];
                data._legController->commands[foot].kdJoint = Mat3<float>::Identity() * 0.2;

            }else{ // using WBC
                data._legController->commands[foot].pDes = pDesLeg;
                data._legController->commands[foot].vDes = vDesLeg;
                data._legController->commands[foot].kpCartesian = 0.* Kp_stance;
                data._legController->commands[foot].kdCartesian = Kd_stance;
            }
            se_contactState[foot] = contactState;

        }
    }

    data._stateEstimator->setContactPhase(se_contactState);

    // Update For WBC
    pBody_des[0] = world_position_desired[0];
    pBody_des[1] = world_position_desired[1];
    pBody_des[2] = _body_height;

    vBody_des[0] = v_des_world[0];
    vBody_des[1] = v_des_world[1];
    vBody_des[2] = 0.;

    aBody_des.setZero();

    pBody_RPY_des[0] = 0.;
    pBody_RPY_des[1] = 0.;
    pBody_RPY_des[2] = _yaw_des;

    vBody_Ori_des[0] = 0.;
    vBody_Ori_des[1] = 0.;
    vBody_Ori_des[2] = _yaw_turn_rate;

    contact_state = gait->getContactState();
//    Fix wbc bug --WYN
    if (gaitNumber == 4)
        for (int i = 0; i < 4; ++i) {
            contact_state[i] = true;
        }
    // END of WBC Update


}

template<>
void ConvexMPCLocomotion::run(ControlFSMData<double>& data) {
    (void)data;
    printf("call to old CMPC with double!\n");

}

void ConvexMPCLocomotion::updateMPCIfNeeded(int *mpcTable, ControlFSMData<float> &data, bool omniMode) {
    //iterationsBetweenMPC = 30;
    if((iterationCounter % iterationsBetweenMPC) == 1)
    {
//        printf("itercounter: %d, dtMPC: %d\n",iterationCounter,iterationsBetweenMPC);
        auto seResult = data._stateEstimator->getResult();
        float* p = seResult.position.data();

        Vec3<float> v_des_robot(_x_vel_des, _y_vel_des,0);
        Vec3<float> v_des_world = omniMode ? v_des_robot : seResult.rBody.transpose() * v_des_robot;

        //printf("Position error: %.3f, integral %.3f\n", pxy_err[0], x_comp_integral);

        if(current_gait == 4)
        {
            float trajInitial[12] = {
                    _roll_des,
                    _pitch_des,
                    (float)stand_traj[5],
                    (float)stand_traj[0],
                    (float)stand_traj[1],
                    (float)_body_height,
                    0,0,0,0,0,0};

            for(int i = 0; i < horizonLength; i++)
                for(int j = 0; j < 12; j++)
                    trajAll[12*i+j] = trajInitial[j];
        }

        else
        {
            const float max_pos_error = .1;
            float xStart = world_position_desired[0];
            float yStart = world_position_desired[1];

            if(xStart - p[0] > max_pos_error) xStart = p[0] + max_pos_error;
            if(p[0] - xStart > max_pos_error) xStart = p[0] - max_pos_error;

            if(yStart - p[1] > max_pos_error) yStart = p[1] + max_pos_error;
            if(p[1] - yStart > max_pos_error) yStart = p[1] - max_pos_error;

            world_position_desired[0] = xStart;
            world_position_desired[1] = yStart;

            float trajInitial[12] = {(float)rpy_comp[0],    // 0
                                     (float)rpy_comp[1],    // 1
                                     _yaw_des,              // 2
                                     xStart,                                   // 3
                                     yStart,                                   // 4
                                     (float)_body_height,                      // 5
                                     0,                                        // 6
                                     0,                                        // 7
                                     _yaw_turn_rate,                           // 8
                                     v_des_world[0],                           // 9
                                     v_des_world[1],                           // 10
                                     0};                                       // 11

            for(int i = 0; i < horizonLength; i++)
            {
                for(int j = 0; j < 12; j++)
                    trajAll[12*i+j] = trajInitial[j];

                if(i == 0) // start at current position  TODO consider not doing this
                {
                    trajAll[2] = seResult.rpy[2];
                }
                else
                {
                    trajAll[12*i + 3] = trajAll[12 * (i - 1) + 3] + dtMPC * v_des_world[0];
                    trajAll[12*i + 4] = trajAll[12 * (i - 1) + 4] + dtMPC * v_des_world[1];
                    trajAll[12*i + 2] = trajAll[12 * (i - 1) + 2] + dtMPC * _yaw_turn_rate;
                }
            }
        }
        Timer solveTimer;

        if(_parameters->cmpc_use_sparse > 0.5) {
            solveSparseMPC(mpcTable, data);
        } else {
            solveDenseMPC(mpcTable, data);
        }
        //printf("TOTAL SOLVE TIME: %.3f\n", solveTimer.getMs());
    }

}
// Default MPC solver -- DenseMPC
void ConvexMPCLocomotion::solveDenseMPC(int *mpcTable, ControlFSMData<float> &data) {
    auto seResult = data._stateEstimator->getResult();
//    Q, R(alpha) is used as a weight matrix
    float alpha;
    float Q[12];
if (data._quadruped->_robotType == RobotType::IUST){
    //                   roll pitch yaw x  y  z  wx  wy  wz   vx   vy   vz
    float Q_IUST[12] = {1, 1, 1, 2, 2, 50, 0, 0, 1, 1, 1, 1}; // milab
    alpha = 8e-6; // milab
    memcpy(Q,Q_IUST,sizeof(Q_IUST));
} else if (data._quadruped->_robotType == RobotType::MINI_CHEETAH){
    float Q_MINI[12] = {0.25, 0.25, 10, 2, 2, 50, 0, 0, 0.3, 0.2, 0.2, 0.1}; //mini cheetah
    alpha = 4e-5; // mini cheetah
    memcpy(Q,Q_MINI,sizeof(Q_MINI));
}else{
    float Q_CH3[12] = { 1,   1,    1, 1, 1, 50, 0,  0,  1,   1,   1,   1}; // cheetah 3
    alpha = 1e-6; // cheetah 3
    memcpy(Q,Q_CH3,sizeof(Q_CH3));
}
/*    Q R Debugger:*/
//    alpha = data.userParameters->R;
//    std::cout<<"R: "<<alpha<<" Q: ";
//    for (int i = 0; i < 12; ++i) {
//        if (i<3){
//            Q[i] = data.userParameters->Q_ang[i];
//        }else if (i<6){
//            Q[i] = data.userParameters->Q_pos[i-3];
//        }else if (i<9){
//            Q[i] = data.userParameters->Q_ori[i-6];
//        }else {
//            Q[i] = data.userParameters->Q_vel[i-9];
//        }
//        std::cout<<Q[i]<<" ";
//    }std::cout<<"\n";

    float yaw = seResult.rpy[2];
    float* weights = Q;
    float* p = seResult.position.data();
    float* v = seResult.vWorld.data();
    float* w = seResult.omegaWorld.data();
    float* q = seResult.orientation.data();

    float r[12];
    for(int i = 0; i < 12; i++)
        r[i] = pFoot[i%4][i/4]  - seResult.position[i/4];

    //printf("current posistion: %3.f %.3f %.3f\n", p[0], p[1], p[2]);

    if(alpha > 1e-4) {
        std::cout << "Alpha was set too high (" << alpha << ") adjust to 1e-5\n";
        alpha = 1e-6;
    }

    Vec3<float> pxy_act(p[0], p[1], 0);
    Vec3<float> pxy_des(world_position_desired[0], world_position_desired[1], 0);
    float pz_err = p[2] - _body_height;
    Vec3<float> vxy(seResult.vWorld[0], seResult.vWorld[1], 0);

    Timer t1;
    dtMPC = dt * iterationsBetweenMPC;
    if(data._quadruped->_robotType == RobotType::IUST) {
        setup_problem(dtMPC,horizonLength,0.4,180);
    }else if(data._quadruped->_robotType == RobotType::MINI_CHEETAH){
        setup_problem(dtMPC,horizonLength,0.4,120);
    }else if(data._quadruped->_robotType == RobotType::CHEETAH_3){
        setup_problem(dtMPC,horizonLength,0.4,650); //DH
    }
    update_x_drag(x_comp_integral);
    if(vxy[0] > 0.3 || vxy[0] < -0.3) {
        //x_comp_integral += _parameters->cmpc_x_drag * pxy_err[0] * dtMPC / vxy[0];
        x_comp_integral += _parameters->cmpc_x_drag * pz_err * dtMPC / vxy[0];
    }

    //printf("pz err: %.3f, pz int: %.3f\n", pz_err, x_comp_integral);

    update_solver_settings(_parameters->jcqp_max_iter, _parameters->jcqp_rho,
                           _parameters->jcqp_sigma, _parameters->jcqp_alpha, _parameters->jcqp_terminate, _parameters->use_jcqp);
    //t1.stopPrint("Setup MPC");

    Timer t2;
    //cout << "dtMPC: " << dtMPC << "\n";
    if(data._quadruped->_robotType == RobotType::IUST) {
        milab_flag = true;
    }else milab_flag = false;
    update_problem_data_floats(p,v,q,w,r,yaw,weights,trajAll,alpha,mpcTable,milab_flag);
    //t2.stopPrint("Run MPC");
    //printf("MPC Solve time %f ms\n", t2.getMs());

    for(int leg = 0; leg < 4; leg++)
    {
        Vec3<float> f;
        for(int axis = 0; axis < 3; axis++)
            f[axis] = get_solution(leg*3 + axis); //force from ground to leg in world frame
        //  predefine feedforward force
        float legForcePre = 23.*9.81/4;
        if ( std::abs(f[2])==0.f){
          f[2] = legForcePre;
        }
        // force from leg to ground in body frame
        f_ff[leg] = -seResult.rBody * f;
//        printf("[%d F:] %7.3f %7.3f %7.3f\n", leg, f_ff[leg][0], f_ff[leg][1],f_ff[leg][2]);

        // Update for WBC
        Fr_des[leg] = f;
    }
}

void ConvexMPCLocomotion::solveSparseMPC(int *mpcTable, ControlFSMData<float> &data) {
    // X0, contact trajectory, state trajectory, feet, get result!
    (void)mpcTable;
    (void)data;
    auto seResult = data._stateEstimator->getResult();

    std::vector<ContactState> contactStates;
    for(int i = 0; i < horizonLength; i++) {
        contactStates.emplace_back(mpcTable[i*4 + 0], mpcTable[i*4 + 1], mpcTable[i*4 + 2], mpcTable[i*4 + 3]);
    }

    for(int i = 0; i < horizonLength; i++) {
        for(u32 j = 0; j < 12; j++) {
            _sparseTrajectory[i][j] = trajAll[i*12 + j];
        }
    }

    Vec12<float> feet;
    for(u32 foot = 0; foot < 4; foot++) {
        for(u32 axis = 0; axis < 3; axis++) {
            feet[foot*3 + axis] = pFoot[foot][axis] - seResult.position[axis];
        }
    }

    _sparseCMPC.setX0(seResult.position, seResult.vWorld, seResult.orientation, seResult.omegaWorld);
    _sparseCMPC.setContactTrajectory(contactStates.data(), contactStates.size());
    _sparseCMPC.setStateTrajectory(_sparseTrajectory);
    _sparseCMPC.setFeet(feet);
    _sparseCMPC.run();

    Vec12<float> resultForce = _sparseCMPC.getResult();

    for(u32 foot = 0; foot < 4; foot++) {
        Vec3<float> force(resultForce[foot*3], resultForce[foot*3 + 1], resultForce[foot*3 + 2]);
        f_ff[foot] = -seResult.rBody * force;
        Fr_des[foot] = force;
    }
}

void ConvexMPCLocomotion::initSparseMPC() { // For MINI Cheetah
    Mat3<double> baseInertia;
    baseInertia << 0.07, 0, 0,
            0, 0.26, 0,
            0, 0, 0.242;
    double mass = 9;
    double maxForce = 120;

    std::vector<double> dtTraj;
    for(int i = 0; i < horizonLength; i++) {
        dtTraj.push_back(dtMPC);
    }

    Vec12<double> weights;
    weights << 0.25, 0.25, 10, 2, 2, 20, 0, 0, 0.3, 0.2, 0.2, 0.2;
    //weights << 0,0,0,1,1,10,0,0,0,0.2,0.2,0;

    _sparseCMPC.setRobotParameters(baseInertia, mass, maxForce);
    _sparseCMPC.setFriction(0.4);
    _sparseCMPC.setWeights(weights, 4e-5);
    _sparseCMPC.setDtTrajectory(dtTraj);

    _sparseTrajectory.resize(horizonLength);
}

void ConvexMPCLocomotion::initMilabSparseMPC() { //For Milab robot
    Mat3<double> baseInertia;
//    baseInertia << 0.1084, 0, 0,   //28kg
//                    0, 0.834, 0,
//                    0, 0, 0.834;

    baseInertia << 0.0891, 0, 0,   //23kg
                    0, 0.685, 0,
                    0, 0, 0.685;

//    baseInertia << 0.0996, 0, 0,   //25.7kg
//                    0, 0.765, 0,
//                    0, 0, 0.765;

    double mass = 23;//25.7;
    double maxForce = 180;

    std::vector<double> dtTraj;
    for(int i = 0; i < horizonLength; i++) {
        dtTraj.push_back(dtMPC);
    }

    Vec12<double> weights;
    weights <<1,  1 , 1,  2,  2,  50,   0,  0,  1,   1,   1,  1;

    _sparseCMPC.setRobotParameters(baseInertia, mass, maxForce);
    _sparseCMPC.setFriction(0.4);
    _sparseCMPC.setWeights(weights, 8e-6);
    _sparseCMPC.setDtTrajectory(dtTraj);

    _sparseTrajectory.resize(horizonLength);
}
