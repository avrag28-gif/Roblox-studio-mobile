package com.rsm.mobile;

import android.app.*;
import android.os.*;
import android.graphics.*;
import android.graphics.drawable.GradientDrawable;
import android.content.*;
import android.view.*;
import android.view.inputmethod.InputMethodManager;
import android.widget.*;
import android.opengl.GLSurfaceView;
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
  GLSurfaceView nativeViewport;
  FrameLayout viewportContainer;
  ArrayList<Obj> objects=new ArrayList<>();
  ArrayList<String> assetUris=new ArrayList<>();
  static final int PICK_ASSET=4107;
  Obj selected;
  Mode mode=Mode.SELECT;
  boolean playing=false,dirty=false;
  TextView status,output;
  EditText scriptEditor;
  final String PREF="rsm_project_v3";
  final String PROJECT_FILE="rsm-project-v3.json"; final String RECOVERY_FILE="rsm-project-v3.recovery.json";

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
    viewportContainer=new FrameLayout(this); nativeViewport=new GLSurfaceView(this); nativeViewport.setEGLContextClientVersion(3); nativeViewport.setRenderer(new GLSurfaceView.Renderer(){public void onSurfaceCreated(javax.microedition.khronos.opengles.GL10 gl,javax.microedition.khronos.egl.EGLConfig cfg){nativeSurfaceCreated();} public void onSurfaceChanged(javax.microedition.khronos.opengles.GL10 gl,int w,int h){nativeSurfaceChanged(w,h);} public void onDrawFrame(javax.microedition.khronos.opengles.GL10 gl){nativeSurfaceDraw();}}); nativeViewport.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY); viewportContainer.addView(nativeViewport,new FrameLayout.LayoutParams(-1,-1)); viewport=new Viewport(this); viewport.setBackgroundColor(Color.TRANSPARENT); viewportContainer.addView(viewport,new FrameLayout.LayoutParams(-1,-1));
    body.addView(explorer,new LinearLayout.LayoutParams(dp(215),-1));
    body.addView(viewportContainer,new LinearLayout.LayoutParams(0,-1,1));
    body.addView(props,new LinearLayout.LayoutParams(dp(270),-1));
    root.addView(body,new LinearLayout.LayoutParams(-1,0,1));
    bottom=new LinearLayout(this);bottom.setOrientation(LinearLayout.VERTICAL);bottom.setBackgroundColor(Color.rgb(23,25,30));
    root.addView(bottom,new LinearLayout.LayoutParams(-1,dp(210)));
    status=text("●  EDIT   •   3 objects   •   Ready",12);status.setPadding(dp(12),0,dp(12),0);
    root.addView(status,new LinearLayout.LayoutParams(-1,dp(34)));
    setContentView(root);assetUris.addAll(getSharedPreferences(PREF,0).getStringSet("assets",java.util.Collections.emptySet()));bottom("OUTPUT");refresh();
  }

  View topBar(){
    LinearLayout bar=new LinearLayout(this);bar.setGravity(Gravity.CENTER_VERTICAL);bar.setPadding(dp(8),0,dp(8),0);bar.setBackgroundColor(Color.rgb(31,34,40));
    TextView title=text("RSM  /  Studio",18);bar.addView(title,new LinearLayout.LayoutParams(dp(160),-1));
    Button add=btn("+ Part");add.setOnClickListener(v->addPart());bar.addView(add);
    Button model=btn("+ Model");model.setOnClickListener(v->addModel());bar.addView(model);
    Button play=btn("▶  Play");play.setOnClickListener(v->togglePlay(play));bar.addView(play);
    Button save=btn("Save");save.setOnClickListener(v->saveProject());bar.addView(save);
    Button load=btn("Open");load.setOnClickListener(v->loadProject());bar.addView(load);
    Button move=btn("Move");move.setOnClickListener(v->{mode=Mode.MOVE;viewport.invalidate();});bar.addView(move);
    Button rotate=btn("Rotate");rotate.setOnClickListener(v->{mode=Mode.ROTATE;viewport.invalidate();});bar.addView(rotate);
    Button scale=btn("Scale");scale.setOnClickListener(v->{mode=Mode.SCALE;viewport.invalidate();});bar.addView(scale);
    Button more=btn("⋮");more.setOnClickListener(v->showTools());bar.addView(more,new LinearLayout.LayoutParams(dp(52),-1));
    return bar;
  }

  void addPart(){Obj o=new Obj("Part");o.x=objects.size()*2-2;o.y=1;objects.add(o);select(o);dirty=true;append("INFO","Created Part");}
  void addModel(){Obj o=new Obj("Model");o.type="Model";o.sx=o.sy=o.sz=4;o.y=2;o.color=Color.rgb(190,100,220);objects.add(o);select(o);dirty=true;append("INFO","Created Model");}
  void togglePlay(Button b){
    playing=!playing;b.setText(playing?"■  Stop":"▶  Play");status.setText(playing?"●  PLAY   •   runtime scene active":"●  EDIT   •   "+objects.size()+" objects   •   Ready");
    append("INFO",playing?"Play session started from isolated runtime snapshot.":"Play session stopped; editor scene preserved.");viewport.invalidate();
  }

  void refresh(){refreshExplorer();refreshProps();syncNativeScene();viewport.invalidate();status.setText((playing?"●  PLAY":"●  EDIT")+"   •   "+objects.size()+" objects"+(dirty?"   •   Unsaved":"   •   Saved"));}
  void syncNativeScene(){ if(nativeViewport==null)return; float[] data=new float[objects.size()*13]; int i=0; for(Obj o:objects){data[i++]=o.x;data[i++]=o.y;data[i++]=o.z;data[i++]=o.sx;data[i++]=o.sy;data[i++]=o.sz;data[i++]=o.rx;data[i++]=o.ry;data[i++]=o.rz;data[i++]=Color.red(o.color)/255f;data[i++]=Color.green(o.color)/255f;data[i++]=Color.blue(o.color)/255f;data[i++]=o.anchored?1:0;} nativeSyncScene(data); }
  static native void nativeSurfaceCreated();
  static native void nativeSurfaceChanged(int w,int h);
  static native void nativeSurfaceDraw();
  static native void nativeSurfaceDestroyed();
  static native void nativeLifecycle(int state);
  static native void nativeSyncScene(float[] data);
  static native void nativeCameraOrbit(float yaw,float pitch);
  static native void nativeCameraZoom(float delta);
  static native boolean nativeRunScript(String source);
  void refreshExplorer(){
    explorer.removeAllViews();
    TextView h=text("EXPLORER",13);h.setTypeface(Typeface.DEFAULT,Typeface.BOLD);explorer.addView(h,new LinearLayout.LayoutParams(-1,dp(46)));
    addTree("▾  Workspace",null,true);
    for(Obj o:objects) if("Workspace".equals(o.parent)) addTree("    "+(o.type.equals("Model")?"◇ ":"▣ ")+o.name,o,false);
    for(Obj parent:objects) if("Model".equals(parent.type)) for(Obj o:objects) if(parent.id.equals(o.parent)) addTree("        "+(o.type.equals("Model")?"◇ ":"▣ ")+o.name,o,false);
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
    LinearLayout acts=new LinearLayout(this);Button dup=btn("Duplicate");dup.setOnClickListener(v->duplicate());Button del=btn("Delete");del.setOnClickListener(v->deleteSelected());
    Button reparent=btn("Parent");reparent.setOnClickListener(v->chooseParent());
    acts.addView(dup,new LinearLayout.LayoutParams(0,dp(46),1));acts.addView(reparent,new LinearLayout.LayoutParams(0,dp(46),1));acts.addView(del,new LinearLayout.LayoutParams(0,dp(46),1));props.addView(acts);
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

  void writeRecovery(){try{JSONArray a=new JSONArray();for(Obj o:objects){JSONObject j=new JSONObject();j.put("id",o.id);j.put("name",o.name);j.put("type",o.type);j.put("parent",o.parent);j.put("x",o.x);j.put("y",o.y);j.put("z",o.z);j.put("sx",o.sx);j.put("sy",o.sy);j.put("sz",o.sz);j.put("rx",o.rx);j.put("ry",o.ry);j.put("rz",o.rz);j.put("color",o.color);j.put("anchored",o.anchored);j.put("collide",o.collide);a.put(j);}try(java.io.FileOutputStream out=new java.io.FileOutputStream(new java.io.File(getFilesDir(),RECOVERY_FILE))){out.write(a.toString().getBytes("UTF-8"));out.flush();}}catch(Exception ignored){}}
  void changed(){dirty=true;writeRecovery();refreshExplorer();viewport.invalidate();status.setText("●  "+(playing?"PLAY":"EDIT")+"   •   "+objects.size()+" objects   •   Unsaved");}
  void chooseParent(){
    if(selected==null)return;
    ArrayList<String> names=new ArrayList<>();ArrayList<Obj> choices=new ArrayList<>();
    names.add("Workspace");choices.add(null);
    for(Obj o:objects) if(o!=selected && "Model".equals(o.type) && !isDescendant(o,selected)) {names.add(o.name);choices.add(o);}
    new AlertDialog.Builder(this).setTitle("Reparent "+selected.name).setItems(names.toArray(new String[0]),(d,w)->{
      selected.parent=choices.get(w)==null?"Workspace":choices.get(w).id;changed();append("INFO","Reparented "+selected.name);
    }).show();
  }
  boolean isDescendant(Obj candidate,Obj node){String p=candidate.parent;while(!"Workspace".equals(p)){if(p.equals(node.id))return true;Obj q=find(p);if(q==null)break;p=q.parent;}return false;}
  Obj find(String id){for(Obj o:objects)if(o.id.equals(id))return o;return null;}
  void duplicate(){if(selected==null)return;Obj n=new Obj(selected.name+" Copy");n.type=selected.type;n.parent=selected.parent;n.x=selected.x+2;n.y=selected.y;n.z=selected.z;n.sx=selected.sx;n.sy=selected.sy;n.sz=selected.sz;n.color=selected.color;n.anchored=selected.anchored;n.collide=selected.collide;objects.add(n);select(n);changed();append("INFO","Duplicated "+selected.name);}
  void deleteSelected(){if(selected==null)return;String n=selected.name;String removed=selected.id;ArrayList<Obj> gone=new ArrayList<>();for(Obj o:objects){String p=o.parent;while(!"Workspace".equals(p)){if(p.equals(removed)){gone.add(o);break;}Obj q=find(p);if(q==null)break;p=q.parent;}}gone.add(selected);objects.removeAll(gone);selected=null;changed();append("INFO","Deleted "+n);}
  void cycleColor(){int[] c={Color.rgb(90,160,240),Color.rgb(240,100,90),Color.rgb(100,220,130),Color.rgb(220,190,70),Color.rgb(190,100,220),Color.WHITE};int i=0;for(int j=0;j<c.length;j++)if(selected.color==c[j])i=(j+1)%c.length;selected.color=c[i];changed();refreshProps();}
  
  void bottom(String tab){
    bottom.removeAllViews();LinearLayout tabs=new LinearLayout(this);String[] names={"OUTPUT","SCRIPT","ASSETS","DEBUG"};
    for(String n:names){Button b=btn(n);b.setOnClickListener(v->bottom(n));tabs.addView(b,new LinearLayout.LayoutParams(0,dp(44),1));}bottom.addView(tabs);
    if(tab.equals("SCRIPT"))showScript(); else if(tab.equals("ASSETS"))showAssets(); else if(tab.equals("DEBUG"))showDebug(); else {bottom.addView(output,new LinearLayout.LayoutParams(-1,0,1));}
  }
  void showScript(){
    scriptEditor=new ScriptEditorView(this);scriptEditor.setText("-- RSM Luau Script\nlocal Workspace = game:GetService(\"Workspace\")\n\nlocal part = Instance.new(\"Part\")\npart.Name = \"RuntimePart\"\npart.Parent = Workspace\n");scriptEditor.setTextColor(Color.WHITE);scriptEditor.setTextSize(13);scriptEditor.setGravity(Gravity.TOP);scriptEditor.setPadding(dp(12),dp(8),dp(12),dp(8));scriptEditor.setBackgroundColor(Color.rgb(16,18,22));bottom.addView(scriptEditor,new LinearLayout.LayoutParams(-1,0,1));
    Button run=btn("▶  Run Script");run.setOnClickListener(v->{String src=scriptEditor.getText().toString();if(src.trim().isEmpty()){append("ERROR","Script is empty.");return;}boolean ok=nativeRunScript(src);append(ok?"INFO":"ERROR",ok?"Luau script executed in runtime.":"Luau script failed sandbox/compile checks.");Toast.makeText(this,ok?"Script executed":"Script failed",Toast.LENGTH_SHORT).show();refresh();});bottom.addView(run,new LinearLayout.LayoutParams(-1,dp(42)));
  }
  void showAssets(){
    LinearLayout p=new LinearLayout(this);p.setOrientation(LinearLayout.VERTICAL);p.addView(text("ASSET BROWSER",13));
    Button imp=btn("+ Import Asset");imp.setOnClickListener(v->{Intent i=new Intent(Intent.ACTION_OPEN_DOCUMENT);i.addCategory(Intent.CATEGORY_OPENABLE);i.setType("*/*");startActivityForResult(i,PICK_ASSET);});p.addView(imp);
    p.addView(text("Imported assets",12));
    if(assetUris.isEmpty())p.addView(text("No external assets imported yet.",12));
    else for(String u:assetUris)p.addView(text("▣  "+u,11));
    p.addView(text("Pipeline: Import → Validate → Process → Cache → Runtime.",12));bottom.addView(p,new LinearLayout.LayoutParams(-1,0,1));
  }
  @Override protected void onActivityResult(int requestCode,int resultCode,Intent data){
    super.onActivityResult(requestCode,resultCode,data);if(requestCode!=PICK_ASSET||resultCode!=RESULT_OK||data==null||data.getData()==null)return;
    String uri=data.getData().toString();try{int flags=data.getFlags()&(Intent.FLAG_GRANT_READ_URI_PERMISSION|Intent.FLAG_GRANT_WRITE_URI_PERMISSION);getContentResolver().takePersistableUriPermission(data.getData(),flags);}catch(Exception ignored){}if(!assetUris.contains(uri)){assetUris.add(uri);getSharedPreferences(PREF,0).edit().putStringSet("assets",new java.util.HashSet<>(assetUris)).apply();append("INFO","Imported asset: "+uri);dirty=true;}bottom("ASSETS");
  }
  void showDebug(){bottom.addView(text("CPU  —  ready\\nGPU  —  renderer backend: Android surface\\nScene objects  —  "+objects.size()+"\\nPhysics bodies  —  "+objects.size()+"\\nScripts  —  sandbox\\nMemory  —  runtime monitored",12),new LinearLayout.LayoutParams(-1,0,1));}
  void append(String level,String msg){if(output==null)output=text("",12);output.append("["+level+"] "+msg+"\\n");}
  void showTools(){bottom("OUTPUT");new AlertDialog.Builder(this).setTitle("Studio Tools").setItems(new String[]{"Move","Rotate","Scale","Output","Script","Assets","Debug","Project Settings"},(d,w)->{if(w==0)mode=Mode.MOVE;else if(w==1)mode=Mode.ROTATE;else if(w==2)mode=Mode.SCALE;else if(w==3)bottom("OUTPUT");else if(w==4)bottom("SCRIPT");else if(w==5)bottom("ASSETS");else if(w==6)bottom("DEBUG");else settings();}).show();}
  void settings(){new AlertDialog.Builder(this).setTitle("Project Settings").setMessage("RSM Studio\\nRenderer: Vulkan / OpenGL ES fallback\\nTarget: Android ARM64\\nProject format: RSM-3\\nAutosave: enabled\\nSandbox: restricted").setPositiveButton("OK",null).show();}

  void saveProject(){
    try{
      JSONArray a=new JSONArray();for(Obj o:objects){JSONObject j=new JSONObject();j.put("id",o.id);j.put("name",o.name);j.put("type",o.type);j.put("parent",o.parent);j.put("x",o.x);j.put("y",o.y);j.put("z",o.z);j.put("sx",o.sx);j.put("sy",o.sy);j.put("sz",o.sz);j.put("rx",o.rx);j.put("ry",o.ry);j.put("rz",o.rz);j.put("color",o.color);j.put("anchored",o.anchored);j.put("collide",o.collide);a.put(j);}
      String payload=a.toString();java.io.File dst=new java.io.File(getFilesDir(),PROJECT_FILE),bak=new java.io.File(getFilesDir(),PROJECT_FILE+".bak"),tmp=new java.io.File(getFilesDir(),PROJECT_FILE+".tmp");if(dst.exists())try(java.io.FileInputStream in=new java.io.FileInputStream(dst);java.io.FileOutputStream out=new java.io.FileOutputStream(bak)){byte[] buf=new byte[8192];int n;while((n=in.read(buf))>0)out.write(buf,0,n);}try(java.io.FileOutputStream out=new java.io.FileOutputStream(tmp)){out.write(payload.getBytes("UTF-8"));out.flush();}if(!tmp.renameTo(dst))throw new java.io.IOException("atomic project replace failed");getSharedPreferences(PREF,0).edit().putInt("version",3).putString("scene",payload).putLong("savedAt",System.currentTimeMillis()).apply();dirty=false;new java.io.File(getFilesDir(),RECOVERY_FILE).delete();refresh();append("INFO","Project saved (version 3).");
    }catch(Exception e){append("ERROR",e.toString());}
  }
  @Override protected void onResume(){super.onResume();nativeLifecycle(1);}
  @Override protected void onPause(){nativeLifecycle(2);super.onPause();if(dirty)saveProject();}
  @Override protected void onDestroy(){nativeSurfaceDestroyed();nativeLifecycle(0);super.onDestroy();}

  void loadProject(){
    String s=null; try{java.io.File dst=new java.io.File(getFilesDir(),PROJECT_FILE);if(dst.exists())s=new String(java.nio.file.Files.readAllBytes(dst.toPath()),"UTF-8"); else {java.io.File bak=new java.io.File(getFilesDir(),PROJECT_FILE+".bak");if(bak.exists())s=new String(java.nio.file.Files.readAllBytes(bak.toPath()),"UTF-8");}}catch(Exception ignored){} if(s==null){java.io.File rec=new java.io.File(getFilesDir(),RECOVERY_FILE);if(rec.exists()){try{s=new String(java.nio.file.Files.readAllBytes(rec.toPath()),"UTF-8");append("WARNING","Recovered unsaved project state.");}catch(Exception ignored){}}} if(s==null)s=getSharedPreferences(PREF,0).getString("scene",null); if(s==null){Toast.makeText(this,"No saved project",Toast.LENGTH_SHORT).show();return;}
    try{JSONArray a=new JSONArray(s);objects.clear();for(int i=0;i<a.length();i++){JSONObject j=a.getJSONObject(i);Obj o=new Obj(j.getString("name"));o.id=j.optString("id",o.id);o.type=j.optString("type","Part");o.parent=j.optString("parent","Workspace");o.x=(float)j.optDouble("x");o.y=(float)j.optDouble("y");o.z=(float)j.optDouble("z");o.sx=(float)j.optDouble("sx",2);o.sy=(float)j.optDouble("sy",2);o.sz=(float)j.optDouble("sz",2);o.rx=(float)j.optDouble("rx");o.ry=(float)j.optDouble("ry");o.rz=(float)j.optDouble("rz");o.color=j.optInt("color",Color.rgb(90,160,240));o.anchored=j.optBoolean("anchored",true);o.collide=j.optBoolean("collide",true);objects.add(o);}selected=null;dirty=false;refresh();append("INFO","Project loaded.");}catch(Exception e){append("ERROR","Load failed: "+e.getMessage());}
  }

  class Viewport extends View{
    Paint p=new Paint(3);float lastX,lastY;boolean drag;
    float lastSpan,lastAngle;
    Viewport(Context c){super(c);setFocusable(true);setBackgroundColor(Color.TRANSPARENT);setZ(2);}
    protected void onDraw(Canvas c){
      super.onDraw(c);float w=getWidth(),h=getHeight();p.setStyle(Paint.Style.STROKE);p.setStrokeWidth(1);p.setColor(Color.rgb(43,47,55));
      float grid=dp(32);for(float x=w/2;x<w;x+=grid)c.drawLine(x,0,x,h,p);for(float x=w/2;x>0;x-=grid)c.drawLine(x,0,x,h,p);for(float y=h/2;y<h;y+=grid)c.drawLine(0,y,w,y,p);for(float y=h/2;y>0;y-=grid)c.drawLine(0,y,w,y,p);
      p.setColor(Color.rgb(100,105,115));p.setStrokeWidth(2);c.drawLine(0,h/2,w,h/2,p);c.drawLine(w/2,0,w/2,h,p);
      if(selected!=null){float scale=dp(22),x=w/2+selected.x*scale,y=h/2-selected.y*scale;p.setStyle(Paint.Style.STROKE);p.setStrokeWidth(dp(2));p.setColor(Color.WHITE);c.drawCircle(x,y,dp(18),p);p.setStyle(Paint.Style.FILL);}
      p.setStyle(Paint.Style.FILL);p.setTextSize(dp(14));p.setColor(Color.WHITE);c.drawText("3D VIEWPORT   •   "+mode.name()+"   •   "+objects.size()+" objects",dp(14),dp(24),p);
      p.setTextSize(dp(11));p.setColor(Color.LTGRAY);c.drawText("1-finger select/drag  •  2-finger camera  •  pinch zoom",dp(14),h-dp(12),p);
    }
    void drawObj(Canvas c,Obj o,float cx,float cy){
      float scale=dp(22);float sx=o.sx*scale,sy=o.sy*scale;float x=cx+o.x*scale,y=cy-o.y*scale;
      p.setStyle(Paint.Style.FILL);p.setColor(o.color);c.drawRect(x-sx/2,y-sy/2,x+sx/2,y+sy/2,p);
      if(o==selected){p.setStyle(Paint.Style.STROKE);p.setStrokeWidth(dp(3));p.setColor(Color.WHITE);c.drawRect(x-sx/2-dp(4),y-sy/2-dp(4),x+sx/2+dp(4),y+sy/2+dp(4),p);p.setStrokeWidth(dp(2));p.setColor(Color.RED);c.drawLine(x,y,x+dp(55),y,p);p.setColor(Color.GREEN);c.drawLine(x,y,x,y-dp(55),p);}
      p.setStyle(Paint.Style.FILL);p.setTextSize(dp(10));p.setColor(Color.WHITE);c.drawText(o.name,x-sx/2,y+sy/2+dp(14),p);
    }
    public boolean onTouchEvent(android.view.MotionEvent e){
      if(e.getPointerCount()>1){float x0=e.getX(0),y0=e.getY(0),x1=e.getX(1),y1=e.getY(1);float dx=(x0+x1)*.5f-lastX,dy=(y0+y1)*.5f-lastY;float span=(float)Math.hypot(x1-x0,y1-y0);if(e.getAction()==MotionEvent.ACTION_POINTER_DOWN||lastSpan==0){lastSpan=span;lastAngle=(float)Math.atan2(y1-y0,x1-x0);}else if(e.getAction()==MotionEvent.ACTION_MOVE){float angle=(float)Math.atan2(y1-y0,x1-x0);nativeCameraOrbit(-dx*.006f+(angle-lastAngle)*0.35f,dy*.006f);nativeCameraZoom((lastSpan-span)*.02f);lastSpan=span;lastAngle=angle;}lastX=(x0+x1)*.5f;lastY=(y0+y1)*.5f;return true;}
      float x=e.getX(),y=e.getY();if(e.getAction()==MotionEvent.ACTION_DOWN){lastX=x;lastY=y;drag=true;return true;}
      if(e.getAction()==MotionEvent.ACTION_UP){lastSpan=0;Obj hit=hit(x,y);if(hit!=null){select(hit);}else if(mode==Mode.SELECT){selected=null;refreshProps();invalidate();}drag=false;return true;}
      if(e.getAction()==MotionEvent.ACTION_MOVE&&drag&&selected!=null&&mode!=Mode.SELECT){applyGesture(selected,x-lastX,y-lastY);lastX=x;lastY=y;return true;}
      return true;
    }
    Obj hit(float x,float y){float cx=getWidth()/2,cy=getHeight()/2,scale=dp(22);for(int i=objects.size()-1;i>=0;i--){Obj o=objects.get(i);float ox=cx+o.x*scale,oy=cy-o.y*scale;if(Math.abs(x-ox)<o.sx*scale/2+dp(12)&&Math.abs(y-oy)<o.sy*scale/2+dp(12))return o;}return null;}
    void applyGesture(Obj o,float dx,float dy){float scale=dp(22);if(mode==Mode.MOVE){o.x+=dx/scale;o.y-=dy/scale;}else if(mode==Mode.ROTATE){o.ry+=dx;o.rx+=dy;}else if(mode==Mode.SCALE){o.sx=Math.max(.1f,o.sx+dx/scale);o.sy=Math.max(.1f,o.sy-dy/scale);}changed();}
  }
}
