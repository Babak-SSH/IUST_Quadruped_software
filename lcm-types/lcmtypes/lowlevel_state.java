/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package lcmtypes;
 
import java.io.*;
import java.util.*;
import lcm.lcm.*;
 
public final class lowlevel_state implements lcm.lcm.LCMEncodable
{
    public float q[];
    public float qd[];
    public float p[];
    public float v[];
    public float tau_est[];
    public float rpy[];
    public float position[];
    public float vWorld[];
    public float vBody[];
    public float aWorld[];
    public float aBody[];
    public float omegaBody[];
    public float omegaWorld[];
    public float quat[];
 
    public lowlevel_state()
    {
        q = new float[12];
        qd = new float[12];
        p = new float[12];
        v = new float[12];
        tau_est = new float[12];
        rpy = new float[3];
        position = new float[3];
        vWorld = new float[3];
        vBody = new float[3];
        aWorld = new float[3];
        aBody = new float[3];
        omegaBody = new float[3];
        omegaWorld = new float[3];
        quat = new float[4];
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0xf694c633671a3431L;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class<?>>());
    }
 
    public static long _hashRecursive(ArrayList<Class<?>> classes)
    {
        if (classes.contains(lcmtypes.lowlevel_state.class))
            return 0L;
 
        classes.add(lcmtypes.lowlevel_state.class);
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
        for (int a = 0; a < 12; a++) {
            outs.writeFloat(this.q[a]); 
        }
 
        for (int a = 0; a < 12; a++) {
            outs.writeFloat(this.qd[a]); 
        }
 
        for (int a = 0; a < 12; a++) {
            outs.writeFloat(this.p[a]); 
        }
 
        for (int a = 0; a < 12; a++) {
            outs.writeFloat(this.v[a]); 
        }
 
        for (int a = 0; a < 12; a++) {
            outs.writeFloat(this.tau_est[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.rpy[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.position[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.vWorld[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.vBody[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.aWorld[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.aBody[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.omegaBody[a]); 
        }
 
        for (int a = 0; a < 3; a++) {
            outs.writeFloat(this.omegaWorld[a]); 
        }
 
        for (int a = 0; a < 4; a++) {
            outs.writeFloat(this.quat[a]); 
        }
 
    }
 
    public lowlevel_state(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public lowlevel_state(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static lcmtypes.lowlevel_state _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        lcmtypes.lowlevel_state o = new lcmtypes.lowlevel_state();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        this.q = new float[(int) 12];
        for (int a = 0; a < 12; a++) {
            this.q[a] = ins.readFloat();
        }
 
        this.qd = new float[(int) 12];
        for (int a = 0; a < 12; a++) {
            this.qd[a] = ins.readFloat();
        }
 
        this.p = new float[(int) 12];
        for (int a = 0; a < 12; a++) {
            this.p[a] = ins.readFloat();
        }
 
        this.v = new float[(int) 12];
        for (int a = 0; a < 12; a++) {
            this.v[a] = ins.readFloat();
        }
 
        this.tau_est = new float[(int) 12];
        for (int a = 0; a < 12; a++) {
            this.tau_est[a] = ins.readFloat();
        }
 
        this.rpy = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.rpy[a] = ins.readFloat();
        }
 
        this.position = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.position[a] = ins.readFloat();
        }
 
        this.vWorld = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.vWorld[a] = ins.readFloat();
        }
 
        this.vBody = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.vBody[a] = ins.readFloat();
        }
 
        this.aWorld = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.aWorld[a] = ins.readFloat();
        }
 
        this.aBody = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.aBody[a] = ins.readFloat();
        }
 
        this.omegaBody = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.omegaBody[a] = ins.readFloat();
        }
 
        this.omegaWorld = new float[(int) 3];
        for (int a = 0; a < 3; a++) {
            this.omegaWorld[a] = ins.readFloat();
        }
 
        this.quat = new float[(int) 4];
        for (int a = 0; a < 4; a++) {
            this.quat[a] = ins.readFloat();
        }
 
    }
 
    public lcmtypes.lowlevel_state copy()
    {
        lcmtypes.lowlevel_state outobj = new lcmtypes.lowlevel_state();
        outobj.q = new float[(int) 12];
        System.arraycopy(this.q, 0, outobj.q, 0, 12); 
        outobj.qd = new float[(int) 12];
        System.arraycopy(this.qd, 0, outobj.qd, 0, 12); 
        outobj.p = new float[(int) 12];
        System.arraycopy(this.p, 0, outobj.p, 0, 12); 
        outobj.v = new float[(int) 12];
        System.arraycopy(this.v, 0, outobj.v, 0, 12); 
        outobj.tau_est = new float[(int) 12];
        System.arraycopy(this.tau_est, 0, outobj.tau_est, 0, 12); 
        outobj.rpy = new float[(int) 3];
        System.arraycopy(this.rpy, 0, outobj.rpy, 0, 3); 
        outobj.position = new float[(int) 3];
        System.arraycopy(this.position, 0, outobj.position, 0, 3); 
        outobj.vWorld = new float[(int) 3];
        System.arraycopy(this.vWorld, 0, outobj.vWorld, 0, 3); 
        outobj.vBody = new float[(int) 3];
        System.arraycopy(this.vBody, 0, outobj.vBody, 0, 3); 
        outobj.aWorld = new float[(int) 3];
        System.arraycopy(this.aWorld, 0, outobj.aWorld, 0, 3); 
        outobj.aBody = new float[(int) 3];
        System.arraycopy(this.aBody, 0, outobj.aBody, 0, 3); 
        outobj.omegaBody = new float[(int) 3];
        System.arraycopy(this.omegaBody, 0, outobj.omegaBody, 0, 3); 
        outobj.omegaWorld = new float[(int) 3];
        System.arraycopy(this.omegaWorld, 0, outobj.omegaWorld, 0, 3); 
        outobj.quat = new float[(int) 4];
        System.arraycopy(this.quat, 0, outobj.quat, 0, 4); 
        return outobj;
    }
 
}

