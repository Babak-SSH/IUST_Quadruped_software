/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package lcmtypes;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class simulator_lcmt implements lcm.lcm.LCMEncodable
{
    public double vb[];
    public double rpy[];
    public long timesteps;
    public double time;
    public double quat[];
    public double R[][];
    public double omegab[];
    public double omega[];
    public double p[];
    public double v[];
    public double vbd[];
    public double q[][];
    public double qd[][];
    public double qdd[][];
    public double tau[][];
    public double f_foot[][];
    public double p_foot[][];
 
    public simulator_lcmt()
    {
        vb = new double[3];
        rpy = new double[3];
        quat = new double[4];
        R = new double[3][3];
        omegab = new double[3];
        omega = new double[3];
        p = new double[3];
        v = new double[3];
        vbd = new double[3];
        q = new double[4][3];
        qd = new double[4][3];
        qdd = new double[4][3];
        tau = new double[4][3];
        f_foot = new double[4][3];
        p_foot = new double[4][3];
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0x49c5c4ff138274beL;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(lcmtypes.simulator_lcmt.class))
            return 0L;
 
        classes.add(lcmtypes.simulator_lcmt.class);
        long hash = LCM_FINGERPRINT_BASE
            ;
        classes.remove(classes.size() - 1);
        return (hash<<1) + ((hash>>63)&1);
    }
 
    public void encode(DataOutput outs) throws IOException
    {
        outs.writeLong(LCM_FINGERPRINT);
        _encodeRecursive(outs);
    }
 
    public void _encodeRecursive(DataOutput outs) throws IOException
    {
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.vb[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.rpy[a]); 
        }
 
        outs.writeLong(this.timesteps); 
 
        outs.writeDouble(this.time); 
 
        for (int a = 0; a < 4; a++) {
            outs.writeDouble(this.quat[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                outs.writeDouble(this.R[a][b]); 
            }
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.omegab[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.omega[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.p[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.v[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeDouble(this.vbd[a]); 
        }
 
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                outs.writeDouble(this.q[a][b]); 
            }
        }
 
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                outs.writeDouble(this.qd[a][b]); 
            }
        }
 
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                outs.writeDouble(this.qdd[a][b]); 
            }
        }
 
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                outs.writeDouble(this.tau[a][b]); 
            }
        }
 
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                outs.writeDouble(this.f_foot[a][b]); 
            }
        }
 
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                outs.writeDouble(this.p_foot[a][b]); 
            }
        }
 
    }
 
    public simulator_lcmt(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public simulator_lcmt(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static lcmtypes.simulator_lcmt _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        lcmtypes.simulator_lcmt o = new lcmtypes.simulator_lcmt();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.vb = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.vb[a] = ins.readDouble();
        }
 
        this.rpy = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.rpy[a] = ins.readDouble();
        }
 
        this.timesteps = ins.readLong();
 
        this.time = ins.readDouble();
 
        this.quat = new double[(int) 4];
        for (int a = 0; a < 4; a++) {
            this.quat[a] = ins.readDouble();
        }
 
        this.R = new double[(int) 3][(int) 3];
        for (int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                this.R[a][b] = ins.readDouble();
            }
        }
 
        this.omegab = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.omegab[a] = ins.readDouble();
        }
 
        this.omega = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.omega[a] = ins.readDouble();
        }
 
        this.p = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.p[a] = ins.readDouble();
        }
 
        this.v = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.v[a] = ins.readDouble();
        }
 
        this.vbd = new double[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.vbd[a] = ins.readDouble();
        }
 
        this.q = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                this.q[a][b] = ins.readDouble();
            }
        }
 
        this.qd = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                this.qd[a][b] = ins.readDouble();
            }
        }
 
        this.qdd = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                this.qdd[a][b] = ins.readDouble();
            }
        }
 
        this.tau = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                this.tau[a][b] = ins.readDouble();
            }
        }
 
        this.f_foot = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                this.f_foot[a][b] = ins.readDouble();
            }
        }
 
        this.p_foot = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            for (int b = 0; b < 3; b++) {
                this.p_foot[a][b] = ins.readDouble();
            }
        }
 
    }
 
    public lcmtypes.simulator_lcmt copy()
    {
        lcmtypes.simulator_lcmt outobj = new lcmtypes.simulator_lcmt();
        outobj.vb = new double[(int) 3];
        System.arraycopy(this.vb, 0, outobj.vb, 0, 3); 
        outobj.rpy = new double[(int) 3];
        System.arraycopy(this.rpy, 0, outobj.rpy, 0, 3); 
        outobj.timesteps = this.timesteps;
 
        outobj.time = this.time;
 
        outobj.quat = new double[(int) 4];
        System.arraycopy(this.quat, 0, outobj.quat, 0, 4); 
        outobj.R = new double[(int) 3][(int) 3];
        for (int a = 0; a < 3; a++) {
            System.arraycopy(this.R[a], 0, outobj.R[a], 0, 3);        }
 
        outobj.omegab = new double[(int) 3];
        System.arraycopy(this.omegab, 0, outobj.omegab, 0, 3); 
        outobj.omega = new double[(int) 3];
        System.arraycopy(this.omega, 0, outobj.omega, 0, 3); 
        outobj.p = new double[(int) 3];
        System.arraycopy(this.p, 0, outobj.p, 0, 3); 
        outobj.v = new double[(int) 3];
        System.arraycopy(this.v, 0, outobj.v, 0, 3); 
        outobj.vbd = new double[(int) 3];
        System.arraycopy(this.vbd, 0, outobj.vbd, 0, 3); 
        outobj.q = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            System.arraycopy(this.q[a], 0, outobj.q[a], 0, 3);        }
 
        outobj.qd = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            System.arraycopy(this.qd[a], 0, outobj.qd[a], 0, 3);        }
 
        outobj.qdd = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            System.arraycopy(this.qdd[a], 0, outobj.qdd[a], 0, 3);        }
 
        outobj.tau = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            System.arraycopy(this.tau[a], 0, outobj.tau[a], 0, 3);        }
 
        outobj.f_foot = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            System.arraycopy(this.f_foot[a], 0, outobj.f_foot[a], 0, 3);        }
 
        outobj.p_foot = new double[(int) 4][(int) 3];
        for (int a = 0; a < 4; a++) {
            System.arraycopy(this.p_foot[a], 0, outobj.p_foot[a], 0, 3);        }
 
        return outobj;
    }
 
}

