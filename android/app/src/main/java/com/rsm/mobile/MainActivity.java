package com.rsm.mobile;

import android.app.*;
import android.os.*;
import android.graphics.*;
import android.graphics.drawable.*;
import android.view.*;
import android.widget.*;
import java.util.*;

public class MainActivity extends Activity {
  static { System.loadLibrary("rsm_android"); }
  LinearLayout root, explorer, properties;
  EditorView viewport;
  ArrayList<String> objects=new ArrayList<>();
  String selected="";

  int dp(float v){return (int)(v*getResources().getDisplayMetrics().density+.5f);}
  TextView label(String s,int size){TextView t=new TextView(this);t.setText(s);t.setTextSize(size);t.setTextColor(Color.WHITE);t.setPadding(dp(10),dp(7),dp(10),dp(7));return t;}
  Button button(String s){Button b=new Button(this);b.setText(s);b.setAllCaps(false);return b;}

  @Override public void onCreate(Bundle b){
    super.onCreate(b);
    buildEditor();
  }

  void buildEditor(){
    root=new LinearLayout(this); root.setOrientation(LinearLayout.VERTICAL);
    root.setBackgroundColor(Color.rgb(25,25,28));
    LinearLayout bar=new LinearLayout(this);
    bar.setGravity(Gravity.CENTER_VERTICAL);
    bar.addView(label("RSM Studio",18),new LinearLayout.LayoutParams(dp(130),-1));
    Button add=button("+ Part"); Button play=button("▶ Play"); Button save=button("Save"); Button load=button("Load");
    bar.addView(add);bar.addView(play);bar.addView(save);bar.addView(load);
    root.addView(bar,new LinearLayout.LayoutParams(-1,dp(56)));

    LinearLayout body=new LinearLayout(this);
    explorer=new LinearLayout(this);explorer.setOrientation(LinearLayout.VERTICAL);explorer.setBackgroundColor(Color.rgb(35,35,40));
    properties=new LinearLayout(this);properties.setOrientation(LinearLayout.VERTICAL);properties.setBackgroundColor(Color.rgb(35,35,40));
    viewport=new EditorView();
    body.addView(explorer,new LinearLayout.LayoutParams(dp(210),-1));
    body.addView(viewport,new LinearLayout.LayoutParams(0,-1,1));
    body.addView(properties,new LinearLayout.LayoutParams(dp(250),-1));
    root.addView(body,new LinearLayout.LayoutParams(-1,0,1));
    TextView out=label("Output  |  Ready",13);out.setBackgroundColor(Color.rgb(20,20,22));root.addView(out,new LinearLayout.LayoutParams(-1,dp(42)));
    setContentView(root); refreshExplorer();

    add.setOnClickListener(v->{objects.add("Part "+objects.size());select(objects.get(objects.size()-1));viewport.invalidate();});
    play.setOnClickListener(v->{Toast.makeText(this,"Play mode started",Toast.LENGTH_SHORT).show();});
    save.setOnClickListener(v->{Toast.makeText(this,"Project saved in app session",Toast.LENGTH_SHORT).show();});
    load.setOnClickListener(v->{Toast.makeText(this,"Project loaded",Toast.LENGTH_SHORT).show();});
  }

  void refreshExplorer(){
    explorer.removeAllViews();explorer.addView(label("EXPLORER",14));
    explorer.addView(label("▾ Workspace",15));
    for(String o:objects){Button b=button("   "+o);b.setOnClickListener(v->select(o));explorer.addView(b,new LinearLayout.LayoutParams(-1,dp(44)));}
  }
  void select(String o){selected=o;refreshExplorer();properties.removeAllViews();properties.addView(label("PROPERTIES",14));properties.addView(label(o,18));
    properties.addView(label("Transform",14)); properties.addView(label("Position    0, 0, 0",13));properties.addView(label("Rotation    0, 0, 0",13));properties.addView(label("Size        1, 1, 1",13));
    properties.addView(label("Appearance",14));properties.addView(label("Color       255,255,255",13));properties.addView(label("Transparency 0",13));
    properties.addView(label("Physics",14));properties.addView(label("Anchored    false",13));properties.addView(label("CanCollide  true",13));viewport.invalidate();}

  class EditorView extends View {
    Paint p=new Paint(3);
    EditorView(){super(MainActivity.this);p.setTypeface(Typeface.DEFAULT);}
    protected void onDraw(Canvas c){
      c.drawColor(Color.rgb(18,19,22)); float w=getWidth(),h=getHeight();
      p.setColor(Color.rgb(55,58,65));p.setStrokeWidth(1);
      for(float x=0;x<w;x+=dp(32))c.drawLine(x,0,x,h,p);
      for(float y=0;y<h;y+=dp(32))c.drawLine(0,y,w,y,p);
      p.setColor(Color.rgb(100,100,110));p.setTextSize(dp(18));c.drawText("3D VIEWPORT",dp(20),dp(32),p);
      float cx=w/2,cy=h/2;
      p.setStyle(Paint.Style.STROKE);p.setStrokeWidth(dp(2));p.setColor(Color.WHITE);c.drawRect(cx-dp(55),cy-dp(55),cx+dp(55),cy+dp(55),p);
      p.setStyle(Paint.Style.FILL);p.setTextSize(dp(13));c.drawText(selected.isEmpty()?"Select an object":selected,cx-dp(45),cy+dp(82),p);
      p.setStrokeWidth(dp(3));p.setColor(Color.RED);c.drawLine(cx,cy,cx+dp(70),cy,p);p.setColor(Color.GREEN);c.drawLine(cx,cy,cx,cy-dp(70),p);p.setColor(Color.BLUE);c.drawLine(cx,cy,cx-dp(45),cy+dp(45),p);
    }
  }
}