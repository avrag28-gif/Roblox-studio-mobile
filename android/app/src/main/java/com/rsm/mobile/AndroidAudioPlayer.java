package com.rsm.mobile;
import android.media.*;
import java.io.*;
public final class AndroidAudioPlayer {
 private AudioTrack track; private byte[] pcm; private int sampleRate=44100,channels=1; private boolean looping;
 public boolean load(InputStream in)throws IOException{
  byte[] b=readAll(in); if(b.length<44||b[0]!='R'||b[1]!='I'||b[2]!='F'||b[3]!='F'||b[8]!='W'||b[9]!='A'||b[10]!='V'||b[11]!='E')return false;
  int p=12,data=-1,dataLen=0,bits=16; while(p+8<=b.length){String id=new String(b,p,4,"US-ASCII");int n=u32(b,p+4);if("fmt ".equals(id)&&n>=16){channels=u16(b,p+10);sampleRate=u32(b,p+12);bits=u16(b,p+22);}if("data".equals(id)){data=p+8;dataLen=Math.min(n,b.length-data);break;}p+=8+n+(n&1);}
  if(data<0||channels<1||channels>2||sampleRate<8000||sampleRate>192000||bits!=16)return false;pcm=new byte[dataLen];System.arraycopy(b,data,pcm,0,dataLen);return true;
 }
 public void play(){if(pcm==null)return;release();int mask=channels==1?AudioFormat.CHANNEL_OUT_MONO:AudioFormat.CHANNEL_OUT_STEREO;int min=AudioTrack.getMinBufferSize(sampleRate,mask,AudioFormat.ENCODING_PCM_16BIT);track=new AudioTrack(AudioManager.STREAM_MUSIC,sampleRate,mask,AudioFormat.ENCODING_PCM_16BIT,Math.max(min,pcm.length),AudioTrack.MODE_STATIC);track.write(pcm,0,pcm.length);track.setLoopPoints(0,pcm.length/(2*channels),looping?-1:0);track.play();}
 public void pause(){if(track!=null)track.pause();} public void resume(){if(track!=null)track.play();} public void stop(){if(track!=null)track.stop();}
 public void setLooping(boolean v){looping=v;if(track!=null){try{track.setLoopPoints(0,pcm.length/(2*channels),v?-1:0);}catch(Exception ignored){}}}
 public void setVolume(float v){if(track!=null)track.setVolume(Math.max(0,Math.min(1,v)));}
 public void release(){if(track!=null){track.release();track=null;}}
 private static int u16(byte[]b,int p){return (b[p]&255)|((b[p+1]&255)<<8);} private static int u32(byte[]b,int p){return (b[p]&255)|((b[p+1]&255)<<8)|((b[p+2]&255)<<16)|((b[p+3]&255)<<24);}
 private static byte[] readAll(InputStream in)throws IOException{ByteArrayOutputStream o=new ByteArrayOutputStream();byte[]buf=new byte[8192];for(int n;(n=in.read(buf))>=0;)o.write(buf,0,n);return o.toByteArray();}
}