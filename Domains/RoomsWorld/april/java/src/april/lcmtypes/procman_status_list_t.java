/* LCM type definition class file
 * This file was automatically generated by lcm-gen
 * DO NOT MODIFY BY HAND!!!!
 */

package april.lcmtypes;
 
import java.io.*;
import java.nio.*;
import java.util.*;
import lcm.lcm.*;
 
public final class procman_status_list_t implements lcm.lcm.LCMEncodable
{
    public long utime;
    public long received_utime;
    public long received_init_utime;
    public String host;
    public int nprocs;
    public april.lcmtypes.procman_status_t statuses[];
 
    public procman_status_list_t()
    {
    }
 
    public static final long LCM_FINGERPRINT;
    public static final long LCM_FINGERPRINT_BASE = 0x252ae27929502fdaL;
 
    static {
        LCM_FINGERPRINT = _hashRecursive(new ArrayList<Class>());
    }
 
    public static long _hashRecursive(ArrayList<Class> classes)
    {
        if (classes.contains(april.lcmtypes.procman_status_list_t.class))
            return 0L;
 
        classes.add(april.lcmtypes.procman_status_list_t.class);
        long hash = LCM_FINGERPRINT_BASE
             + april.lcmtypes.procman_status_t._hashRecursive(classes)
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
        char[] __strbuf = null;
        outs.writeLong(this.utime); 
 
        outs.writeLong(this.received_utime); 
 
        outs.writeLong(this.received_init_utime); 
 
        __strbuf = new char[this.host.length()]; this.host.getChars(0, this.host.length(), __strbuf, 0); outs.writeInt(__strbuf.length+1); for (int _i = 0; _i < __strbuf.length; _i++) outs.write(__strbuf[_i]); outs.writeByte(0); 
 
        outs.writeInt(this.nprocs); 
 
        for (int a = 0; a < this.nprocs; a++) {
            this.statuses[a]._encodeRecursive(outs); 
        }
 
    }
 
    public procman_status_list_t(byte[] data) throws IOException
    {
        this(new LCMDataInputStream(data));
    }
 
    public procman_status_list_t(DataInput ins) throws IOException
    {
        if (ins.readLong() != LCM_FINGERPRINT)
            throw new IOException("LCM Decode error: bad fingerprint");
 
        _decodeRecursive(ins);
    }
 
    public static april.lcmtypes.procman_status_list_t _decodeRecursiveFactory(DataInput ins) throws IOException
    {
        april.lcmtypes.procman_status_list_t o = new april.lcmtypes.procman_status_list_t();
        o._decodeRecursive(ins);
        return o;
    }
 
    public void _decodeRecursive(DataInput ins) throws IOException
    {
        char[] __strbuf = null;
        this.utime = ins.readLong();
 
        this.received_utime = ins.readLong();
 
        this.received_init_utime = ins.readLong();
 
        __strbuf = new char[ins.readInt()-1]; for (int _i = 0; _i < __strbuf.length; _i++) __strbuf[_i] = (char) (ins.readByte()&0xff); ins.readByte(); this.host = new String(__strbuf);
 
        this.nprocs = ins.readInt();
 
        this.statuses = new april.lcmtypes.procman_status_t[(int) nprocs];
        for (int a = 0; a < this.nprocs; a++) {
            this.statuses[a] = april.lcmtypes.procman_status_t._decodeRecursiveFactory(ins);
        }
 
    }
 
    public april.lcmtypes.procman_status_list_t copy()
    {
        april.lcmtypes.procman_status_list_t outobj = new april.lcmtypes.procman_status_list_t();
        outobj.utime = this.utime;
 
        outobj.received_utime = this.received_utime;
 
        outobj.received_init_utime = this.received_init_utime;
 
        outobj.host = this.host;
 
        outobj.nprocs = this.nprocs;
 
        outobj.statuses = new april.lcmtypes.procman_status_t[(int) nprocs];
        for (int a = 0; a < this.nprocs; a++) {
            outobj.statuses[a] = this.statuses[a].copy();
        }
 
        return outobj;
    }
 
}

