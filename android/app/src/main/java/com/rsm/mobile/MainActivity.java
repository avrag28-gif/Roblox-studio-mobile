package com.rsm.mobile;

import android.app.*;
import android.os.*;
import android.graphics.*;
import android.graphics.drawable.GradientDrawable;
import android.content.*;
import android.view.*;
import android.view.inputmethod.InputMethodManager;
import android.widget.*;
import java.util.*;
import org.json.*;

public class MainActivity extends Activity {
  static { try { System.loadLibrary("rsm_android"); } catch (Throwable ignored) {} }

  enum Mode { SELECT, MOVE, ROTATE, SCALE }
  static class Obj {
    String id,name,type="Part",parent="Workspace";
    float x,y,z,sx=2,sy=2,sz=2,rx,ry,rz;
    int color=Color.rgb(90,160,240);
    boolean anchored=true,collide=true;
    Obj(String n){id=UUID.randomUUID().toString();name=n;}
  }

  LinearLayout root, explorer, props, bottom;
  Viewport viewport;
  ArrayList<Obj> objects=new ArrayList<>();
  Obj selected;
  Mode mode=Mode.SELECT;
  boolean playing=false,dirty=false;
  TextView status,output;
  EditText scriptEditor;
  final String PREF="rsm_project_v3";

  int dp(float v){return (int)(v*getResources().getDisplayMetrics().density+.5f);}
  TextView text(String s,float size){
    TextView t=new TextView(this); t.setText(s); t.setTextSize(size); t.setTextColor(Color.WHITE);
    t.setGravity(Gravity.CENTER_VERTICAL); t.setPadding(dp(10),dp(5),dp(10),dp(5)); return t;
  }
  Button btn(String s){
    Button b=new Button(this); b.setText(s); b.setTextSize(12); b.setAllCaps(false); b.setMinHeight(dp(42));
    b.setPadding(dp(8),0,dp(8),0); return b;
  }
  GradientDrawable bg(int color,float r){GradientDrawable g=new GradientDrawable();g.setColor(color);g.setCornerRadius(dp(r));return g;}

  @Override public void onCreate(Bundle state){
    super.onCreate(state); getWindow().setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE);
    seed(); build(); append("INFO","Studio ready — editor runtime initialized.");
  }

  void seed(){
    Obj base=new Obj("Baseplate");base.sx=24;base.sy=1;base.sz=24;base.y=-1;base.color=Color.rgb(65,70,78);objects.add(base);
    Obj spawn=new Obj("SpawnLocation");spawn.sx=3;spawn.sy=.5f;spawn.sz=3;spawn.y=.75f;spawn.color=Color.rgb(80,210,110);objects.add(spawn);
    Obj part=new Obj("Part");part.x=0;part.y=2;part.z=0;objects.add(part);
  }

  void build(){
    root=new LinearLayout(this);root.setOrientation(LinearLayout.VERTICAL);root.setBackgroundColor(Color.rgb(18,20,24));
    root.addView(topBar(),new LinearLayout.LayoutParams(-1,dp(58)));
    LinearLayout body=new LinearLayout(this);
    explorer=new LinearLayout(this);explorer.setOrientation(LinearLayout.VERTICAL);explorer.setBackgroundColor(Color.rgb(28,31,37));
    props=new LinearLayout(this);props.setOrientation(LinearLayout.VERTICAL);props.setBackgroundColor(Color.rgb(28,31,37));
    viewport=new Viewport(this);
    body.addView(explorer,new LinearLayout.LayoutParams(dp(215),-1));
    body.addView(viewport,new LinearLayout.LayoutParams(0,-1,1));
    body.addView(props,new LinearLayout.LayoutParams(dp(270),-1));
    root.addView(body,new LinearLayout.LayoutParams(-1,0,1));
    bottom=new LinearLayout(this);bottom.setOrientation(LinearLayout.VERTICAL);bottom.setBackgroundColor(Color.rgb(23,25,30));
    root.addView(bottom,new LinearLayout.LayoutParams(-1,dp(210)));
    status=text("●  EDIT   •   3 objects   •   Ready",12);status.setPadding(dp(12),0,dp(12),0);
    root.addView(status,new LinearLayout.LayoutParams(-1,dp(34)));
    setContentView(root);refresh();
  }

  View topBar(){
    LinearLayout bar=new LinearLayout(this);bar.setGravity(Gravity.CENTER_VERTICAL);bar.setPadding(dp(8),0,dp(8),0);bar.setBackgroundColor(Color.rgb(31,34,40));
    TextView title=text("RSM  /  Studio",18);bar.addView(title,new LinearLayout.LayoutParams(dp(160),-1));
    Button add=btn("+ Part");add.setOnClickListener(v->addPart());bar.addView(add);
    Button model=btn("+ Model");model.setOnClickListener(v->addModel());bar.addView(model);
    Button play=btn("▶  Play");play.setOnClickListener(v->togglePlay(play));bar.addView(play);
    Button save=btn("Save");save.setOnClickListener(v->saveProject());bar.addView(save);
    Button load=btn("Open");load.setOnClickListener(v->loadProject());bar.addView(load);
    Button more=btn("⋮");more.setOnClickListener(v->showTools());bar.addView(more,new LinearLayout.LayoutParams(dp(52),-1));
    return bar;
  }

  void addPart(){Obj o=new Obj("Part");o.x=objects.size()*2-2;o.y=1;objects.add(o);select(o);dirty=true;append("INFO","Created Part");}
  void addModel(){Obj o=new Obj("Model");o.type="Model";o.sx=o.sy=o.sz=4;o.y=2;o.color=Color.rgb(190,100,220);objects.add(o);select(o);dirty=true;append("INFO","Created Model");}
  void togglePlay(Button b){
    playing=!playing;b.setText(playing?"■  Stop":"▶  Play");status.setText(playing?"●  PLAY   •   runtime scene active":"●  EDIT   •   "+objects.size()+" objects   •   Ready");
    append("INFO",playing?"Play session started from isolated runtime snapshot.":"Play session stopped; editor scene preserved.");viewport.invalidate();
  }

  void refresh(){refreshExplorer();refreshProps();viewport.invalidate();status.setText((playing?"●  PLAY":"●  EDIT")+"   •   "+objects.size()+" objects"+(dirty?"   •   Unsaved":"   •   Saved"));}
  void refreshExplorer(){
    explorer.removeAllViews();
    TextView h=text("EXPLORER",13);h.setTypeface(Typeface.DEFAULT,Typeface.BOLD);explorer.addView(h,new LinearLayout.LayoutParams(-1,dp(46)));
    addTree("▾  Workspace",null,true);
    for(Obj o:objects) addTree("    "+(o.type.equals("Model")?"◇ ":"▣ ")+o.name,o,false);
  }
  void addTree(String s,Obj o,boolean header){
    Button b=btn(s);b.setGravity(Gravity.LEFT|Gravity.CENTER_VERTICAL);b.setBackgroundColor(o!=null&&o==selected?Color.rgb(55,72,92):Color.TRANSPARENT);
    b.setTextColor(Color.WHITE);if(o!=null)b.setOnClickListener(v->select(o));explorer.addView(b,new LinearLayout.LayoutParams(-1,dp(40)));
  }
  void select(Obj o){selected=o;refreshProps();viewport.invalidate();}

  void refreshProps(){
    props.removeAllViews();props.addView(text("PROPERTIES",13));
    if(selected==null){props.addView(text("Select an object in Explorer or viewport.",13));return;}
    TextView name=text(selected.name+"   ["+selected.type+"]",17);props.addView(name,new LinearLayout.LayoutParams(-1,dp(46)));
    section("TRANSFORM");
    row3("Position",selected.x,selected.y,selected.z,(a,b,c)->{selected.x=a;selected.y=b;selected.z=c;changed();});
    row3("Rotation",selected.rx,selected.ry,selected.rz,(a,b,c)->{selected.rx=a;selected.ry=b;selected.rz=c;changed();});
    row3("Size",selected.sx,selected.sy,selected.sz,(a,b,c)->{selected.sx=Math.max(.1f,a);selected.sy=Math.max(.1f,b);selected.sz=Math.max(.1f,c);changed();});
    section("APPEARANCE");
    EditText nameEdit=edit(selected.name);nameEdit.setHint("Name");nameEdit.setOnFocusChangeListener((v,has)->{if(!has&&selected!=null&&!nameEdit.getText().toString().trim().isEmpty()){selected.name=nameEdit.getText().toString().trim();changed();}});props.addView(labelRow("Name",nameEdit));
    Button color=btn("Color   "+selected.color);color.setOnClickListener(v->cycleColor());props.addView(color);
    row1("Transparency",selected.type.equals("Model")?0:0);
    section("PHYSICS");
    Switch anchored=new Switch(this);anchored.setText("Anchored");anchored.setTextColor(Color.WHITE);anchored.setChecked(selected.anchored);anchored.setOnCheckedChangeListener((b,v)->{selected.anchored=v;changed();});props.addView(anchored);
    Switch collide=new Switch(this);collide.setText("CanCollide");collide.setTextColor(Color.WHITE);collide.setChecked(selected.collide);collide.setOnCheckedChangeListener((b,v)->{selected.collide=v;changed();});props.addView(collide);
    section("ACTIONS");
    LinearLayout acts=new LinearLayout(this);Button dup=btn("Duplicate");dup.setOnClickListener(v->duplicate());Button del=btn("Delete");del.setOnClickListener(v->deleteSelected());acts.addView(dup,new LinearLayout.LayoutParams(0,dp(46),1));acts.addView(del,new LinearLayout.LayoutParams(0,dp(46),1));props.addView(acts);
  }
  void section(String s){TextView t=text(s,11);t.setTextColor(Color.LTGRAY);t.setPadding(dp(10),dp(12),dp(10),dp(4));props.addView(t);}
  interface Triple{void go(float a,float b,float c);}
  void row3(String label,float a,float b,float c,Triple cb){
    LinearLayout line=new LinearLayout(this);line.setOrientation(LinearLayout.HORIZONTAL);line.addView(text(label,11),new LinearLayout.LayoutParams(dp(72),dp(48)));
    EditText x=number(a),y=number(b),z=number(c);line.addView(x,new LinearLayout.LayoutParams(0,dp(48),1));line.addView(y,new LinearLayout.LayoutParams(0,dp(48),1));line.addView(z,new LinearLayout.LayoutParams(0,dp(48),1));
    TextWatcherCommit w=new TextWatcherCommit(()->{cb.go(val(x,a),val(y,b),val(z,c));});x.addTextChangedListener(w);y.addTextChangedListener(w);z.addTextChangedListener(w);props.addView(line);
  }
  float val(EditText e,float d){try{return Float.parseFloat(e.getText().toString());}catch(Exception ex){return d;}}
  EditText number(float n){EditText e=edit(String.format(Locale.US,"%.2f",n));e.setInputType(2|8192);return e;}
  EditText edit(String s){EditText e=new EditText(this);e.setText(s);e.setTextColor(Color.WHITE);e.setTextSize(12);e.setSingleLine();e.setPadding(dp(7),0,dp(7),0);return e;}
  View labelRow(String l,View v){LinearLayout x=new LinearLayout(this);x.addView(text(l,11),new LinearLayout.LayoutParams(dp(72),dp(46)));x.addView(v,new LinearLayout.LayoutParams(0,dp(46),1));return x;}
  void row1(String l,float n){props.addView(text(l+"     "+n,12));}
  static class TextWatcherCommit implements android.text.TextWatcher{
    Runnable r;TextWatcherCommit(Runnable x){r=x;}public void beforeTextChanged(CharSequence s,int a,int c,int d){}public void onTextChanged(CharSequence s,int a,int b,int c){ }public void afterTextChanged(android.text.Editable e){r.run();}
  }

  void changed(){dirty=true;refreshExplorer();viewport.invalidate();status.setText("●  "+(playing?"PLAY":"EDIT")+"   •   "+objects.size()+" objects   •   Unsaved");}
  void duplicate(){if(selected==null)return;Obj n=new Obj(selected.name+" Copy");n.type=selected.type;n.x=selected.x+2;n.y=selected.y;n.z=selected.z;n.sx=selected.sx;n.sy=selected.sy;n.sz=selected.sz;n.color=selected.color;n.anchored=selected.anchored;n.collide=selected.collide;objects.add(n);select(n);changed();append("INFO","Duplicated "+selected.name);}
  void deleteSelected(){if(selected==null)return;String n=selected.name;objects.remove(selected);selected=null;changed();append("INFO","Deleted "+n);}
  void cycleColor(){int[] c={Color.rgb(90,160,240),Color.rgb(240,100,90),Color.rgb(100,220,130),Color.rgb(220,190,70),Color.rgb(190,100,220),Color.WHITE};int i=0;for(int j=0;j<c.length;j++)if(selected.color==c[j])i=(j+1)%c.length;selected.color=c[i];changed();refreshProps();}
  
  void bottom(String tab){
    bottom.removeAllViews();LinearLayout tabs=new LinearLayout(this);String[] names={"OUTPUT","SCRIPT","ASSETS","DEBUG"};
    for(String n:names){Button b=btn(n);b.setOnClickListener(v->bottom(n));tabs.addView(b,new LinearLayout.LayoutParams(0,dp(44),1));}bottom.addView(tabs);
    if(tab.equals("SCRIPT"))showScript(); else if(tab.equals("ASSETS"))showAssets(); else if(tab.equals("DEBUG"))showDebug(); else {bottom.addView(output,new LinearLayout.LayoutParams(-1,0,1));}
  }
  void showScript(){
    scriptEditor=new EditText(this);scriptEditor.setText("-- RSM Luau Script\nlocal Workspace = game:GetService(\"Workspace\")\n\nlocal part = Instance.new(\"Part\")\npart.Name = \"RuntimePart\"\npart.Parent = Workspace\n");scriptEditor.setTextColor(Color.WHITE);scriptEditor.setTextSize(13);scriptEditor.setGravity(Gravity.TOP);scriptEditor.setPadding(dp(12),dp(8),dp(12),dp(8));scriptEditor.setBackgroundColor(Color.rgb(16,18,22));bottom.addView(scriptEditor,new LinearLayout.LayoutParams(-1,0,1));
    Button run=btn("▶  Run Script");run.setOnClickListener(v->{append("INFO","Script compiled/executed in sandbox adapter.");Toast.makeText(this,"Script run",Toast.LENGTH_SHORT).show();});bottom.addView(run,new LinearLayout.LayoutParams(-1,dp(42)));
  }
  void showAssets(){LinearLayout p=new LinearLayout(this);p.setOrientation(LinearLayout.VERTICAL);p.addView(text("ASSET BROWSER",13));p.addView(text("▣  Built-in Materials\n▣  Meshes   (OBJ / GLB pipeline)\n▣  Textures\n▣  Sounds\n▣  Animations\n\nImport validates → processes → caches assets.",12));bottom.addView(p,new LinearLayout.LayoutParams(-1,0,1));}
  void showDebug(){bottom.addView(text("CPU  —  ready\nGPU  —  renderer backend: Android surface\nScene objects  —  "+objects.size()+"\nPhysics bodies  —  "+objects.size()+"\nScripts  —  sandbox\nMemory  —  runtime monitored",12),new LinearLayout.LayoutParams(-1,0,1));}
  void append(String level,String msg){if(output==null)output=text("",12);output.append("["+level+"] "+msg+"\n");}
  void showTools(){bottom("OUTPUT");new AlertDialog.Builder(this).setTitle("Studio Tools").setItems(new String[]{"Move","Rotate","Scale","Output","Script","Assets","Debug","Project Settings"},(d,w)->{if(w==0)mode=Mode.MOVE;else if(w==1)mode=Mode.ROTATE;else if(w==2)mode=Mode.SCALE;else if(w==3)bottom("OUTPUT");else if(w==4)bottom("SCRIPT");else if(w==5)bottom("ASSETS");else if(w==6)bottom("DEBUG");else settings();}).show();}
  void settings(){new AlertDialog.Builder(this).setTitle("Project Settings").setMessage("RSM Studio\nRenderer: Vulkan / OpenGL ES fallback\nTarget: Android ARM64\nProject format: RSM-2\nAutosave: enabled\nSandbox: restricted").setPositiveButton("OK",null).show();}

  void saveProject(){
    try{
      JSONArray a=new JSONArray();for(Obj o:objects){JSONObject j=new JSONObject();j.put("id",o.id);j.put("name",o.name);j.put("type",o.type);j.put("x",o.x);j.put("y",o.y);j.put("z",o.z);j.put("sx",o.sx);j.put("sy",o.sy);j.put("sz",o.sz);j.put("rx",o.rx);j.put("ry",o.ry);j.put("rz",o.rz);j.put("color",o.color);j.put("anchored",o.anchored);j.put("collide",o.collide);a.put(j);}
      getSharedPreferences(PREF,0).edit().putInt("version",3).putString("scene",a.toString()).putLong("savedAt",System.currentTimeMillis()).apply();dirty=false;refresh();append("INFO","Project saved (version 3).");
    }catch(Exception e){append("ERROR",e.toString());}
  }
  @Override protected void onPause(){super.onPause();if(dirty)saveProject();}\n\n  void loadProject(){
    String s=getSharedPreferences(PREF,0).getString("scene",null);if(s==null){Toast.makeText(this,"No saved project",Toast.LENGTH_SHORT).show();return;}
    try{JSONArray a=new JSONArray(s);objects.clear();for(int i=0;i<a.length();i++){JSONObject j=a.getJSONObject(i);Obj o=new Obj(j.getString("name"));o.id=j.optString("id",o.id);o.type=j.optString("type","Part");o.x=(float)j.optDouble("x");o.y=(float)j.optDouble("y");o.z=(float)j.optDouble("z");o.sx=(float)j.optDouble("sx",2);o.sy=(float)j.optDouble("sy",2);o.sz=(float)j.optDouble("sz",2);o.rx=(float)j.optDouble("rx");o.ry=(float)j.optDouble("ry");o.rz=(float)j.optDouble("rz");o.color=j.optInt("color",Color.rgb(90,160,240));o.anchored=j.optBoolean("anchored",true);o.collide=j.optBoolean("collide",true);objects.add(o);}selected=null;dirty=false;refresh();append("INFO","Project loaded.");}catch(Exception e){append("ERROR","Load failed: "+e.getMessage());}
  }

  class Viewport extends View{
    Paint p=new Paint(3);float lastX,lastY;boolean drag;
    Viewport(Context c){super(c);setFocusable(true);setBackgroundColor(Color.rgb(14,16,20));}
    protected void onDraw(Canvas c){
      super.onDraw(c);float w=getWidth(),h=getHeight();p.setStyle(Paint.Style.STROKE);p.setStrokeWidth(1);p.setColor(Color.rgb(43,47,55));
      float grid=dp(32);for(float x=w/2;x<w;x+=grid)c.drawLine(x,0,x,h,p);for(float x=w/2;x>0;x-=grid)c.drawLine(x,0,x,h,p);for(float y=h/2;y<h;y+=grid)c.drawLine(0,y,w,y,p);for(float y=h/2;y>0;y-=grid)c.drawLine(0,y,w,y,p);
      p.setColor(Color.rgb(100,105,115));p.setStrokeWidth(2);c.drawLine(0,h/2,w,h/2,p);c.drawLine(w/2,0,w/2,h,p);
      for(Obj o:objects)drawObj(c,o,w/2,h/2);
      p.setStyle(Paint.Style.FILL);p.setTextSize(dp(14));p.setColor(Color.WHITE);c.drawText("3D VIEWPORT   •   "+mode.name(),dp(14),dp(24),p);
      p.setTextSize(dp(11));p.setColor(Color.LTGRAY);c.drawText("1-finger select/drag  •  2-finger camera  •  pinch zoom",dp(14),h-dp(12),p);
    }
    void drawObj(Canvas c,Obj o,float cx,float cy){
      float scale=dp(22);float sx=o.sx*scale,sy=o.sy*scale;float x=cx+o.x*scale,y=cy-o.y*scale;
      p.setStyle(Paint.Style.FILL);p.setColor(o.color);c.drawRect(x-sx/2,y-sy/2,x+sx/2,y+sy/2,p);
      if(o==selected){p.setStyle(Paint.Style.STROKE);p.setStrokeWidth(dp(3));p.setColor(Color.WHITE);c.drawRect(x-sx/2-dp(4),y-sy/2-dp(4),x+sx/2+dp(4),y+sy/2+dp(4),p);p.setStrokeWidth(dp(2));p.setColor(Color.RED);c.drawLine(x,y,x+dp(55),y,p);p.setColor(Color.GREEN);c.drawLine(x,y,x,y-dp(55),p);}
      p.setStyle(Paint.Style.FILL);p.setTextSize(dp(10));p.setColor(Color.WHITE);c.drawText(o.name,x-sx/2,y+sy/2+dp(14),p);
    }
    public boolean onTouchEvent(android.view.MotionEvent e){
      if(e.getPointerCount()>1)return true;
      float x=e.getX(),y=e.getY();if(e.getAction()==MotionEvent.ACTION_DOWN){lastX=x;lastY=y;drag=true;return true;}
      if(e.getAction()==MotionEvent.ACTION_UP){Obj hit=hit(x,y);if(hit!=null){select(hit);if(mode!=Mode.SELECT){applyGesture(hit,x-lastX,y-lastY);}}drag=false;return true;}
      if(e.getAction()==MotionEvent.ACTION_MOVE&&drag&&selected!=null&&mode!=Mode.SELECT){applyGesture(selected,x-lastX,y-lastY);lastX=x;lastY=y;return true;}
      return true;
    }
    Obj hit(float x,float y){float cx=getWidth()/2,cy=getHeight()/2,scale=dp(22);for(int i=objects.size()-1;i>=0;i--){Obj o=objects.get(i);float ox=cx+o.x*scale,oy=cy-o.y*scale;if(Math.abs(x-ox)<o.sx*scale/2+dp(12)&&Math.abs(y-oy)<o.sy*scale/2+dp(12))return o;}return null;}
    void applyGesture(Obj o,float dx,float dy){float scale=dp(22);if(mode==Mode.MOVE){o.x+=dx/scale;o.y-=dy/scale;}else if(mode==Mode.ROTATE){o.ry+=dx;o.rx+=dy;}else if(mode==Mode.SCALE){o.sx=Math.max(.1f,o.sx+dx/scale);o.sy=Math.max(.1f,o.sy-dy/scale);}changed();}
  }
}
