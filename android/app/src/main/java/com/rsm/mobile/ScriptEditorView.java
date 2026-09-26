package com.rsm.mobile;
import android.content.Context;
import android.graphics.*;
import android.text.*;
import android.text.style.ForegroundColorSpan;
import android.util.AttributeSet;
import android.view.*;
import android.widget.*;
import java.util.regex.*;
public class ScriptEditorView extends EditText {
  private final Paint linePaint=new Paint(1);
  private final String[] keywords={"local","function","end","if","then","else","for","while","do","return","true","false","nil","and","or","not"};
  public ScriptEditorView(Context c){super(c);init();}
  public ScriptEditorView(Context c,AttributeSet a){super(c,a);init();}
  private void init(){setTextColor(Color.WHITE);setTextSize(13);setGravity(Gravity.TOP);setPadding(52,10,12,10);setBackgroundColor(Color.rgb(16,18,22));setTypeface(Typeface.MONOSPACE);addTextChangedListener(new TextWatcher(){public void beforeTextChanged(CharSequence s,int st,int c,int a){}public void onTextChanged(CharSequence s,int st,int b,int c){highlight();}public void afterTextChanged(Editable e){}});}
  private void highlight(){Editable e=getText();if(e==null)return;e.setSpan(new ForegroundColorSpan(Color.WHITE),0,e.length(),Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);Matcher m=Pattern.compile("\\b(local|function|end|if|then|else|for|while|do|return|true|false|nil|and|or|not)\\b").matcher(e);while(m.find())e.setSpan(new ForegroundColorSpan(Color.rgb(100,180,255)),m.start(),m.end(),Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);}
  protected void onDraw(Canvas c){int first=getLayout()==null?0:getLayout().getLineForVertical(getScrollY());int last=getLayout()==null?0:getLayout().getLineForVertical(getScrollY()+getHeight());linePaint.setColor(Color.rgb(100,105,115));linePaint.setTextSize(11);for(int i=first;i<=last&&i<getLineCount();i++){float y=getLayout().getLineTop(i)-getScrollY()+getTextSize();c.drawText(Integer.toString(i+1),10,y,linePaint);}super.onDraw(c);}
}