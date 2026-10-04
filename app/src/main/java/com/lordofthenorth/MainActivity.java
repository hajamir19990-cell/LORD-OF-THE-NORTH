package com.lordofthenorth;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.graphics.*;
import android.graphics.drawable.ColorDrawable;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;
import android.os.Bundle;
import android.view.*;
import java.io.InputStream;
import java.nio.*;
import java.util.*;

public class MainActivity extends Activity {
    GameView game;
    HudView hud;
    SharedPreferences prefs;

    @Override public void onCreate(Bundle b) {
        super.onCreate(b);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        hideSystemUI();
        prefs = getSharedPreferences("north_save", MODE_PRIVATE);
        game = new GameView(this);
        hud = new HudView(this, game);
        android.widget.FrameLayout root = new android.widget.FrameLayout(this);
        root.addView(game, new android.widget.FrameLayout.LayoutParams(-1,-1));
        root.addView(hud, new android.widget.FrameLayout.LayoutParams(-1,-1));
        setContentView(root);
    }

    void hideSystemUI() {
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_FULLSCREEN | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION | View.SYSTEM_UI_FLAG_LAYOUT_STABLE);
    }
    @Override public void onWindowFocusChanged(boolean f){ super.onWindowFocusChanged(f); if(f) hideSystemUI(); }
    @Override protected void onPause(){
        super.onPause();
        if(game!=null) game.pauseRenderer();
    }
    @Override protected void onResume(){
        super.onResume();
        if(game!=null) game.resumeRenderer();
    }

    public static class GameView extends GLSurfaceView {
        final NorthRenderer renderer;
        public GameView(Context c) {
            super(c);
            setEGLContextClientVersion(2);
            setEGLConfigChooser(8, 8, 8, 8, 16, 0);
            setPreserveEGLContextOnPause(false);
            renderer = new NorthRenderer(c.getApplicationContext());
            setRenderer(renderer);
            setRenderMode(RENDERMODE_CONTINUOUSLY);
        }
        void pauseRenderer(){ super.onPause(); }
        void resumeRenderer(){ super.onResume(); }
    }

    public static class HudView extends View {
        final GameView game; final Paint p=new Paint(3); final RectF leftJoy=new RectF(), action=new RectF();
        float joyX,joyY; boolean joyDown; float downX,downY; String dialogueName="", dialogueText=""; boolean dialog=false;
        HudView(Context c, GameView g){ super(c); game=g; p.setTypeface(Typeface.create("sans",Typeface.NORMAL)); setFocusable(true); }
        @Override protected void onDraw(Canvas c){
            super.onDraw(c); int w=getWidth(),h=getHeight();
            p.setStyle(Paint.Style.FILL); p.setColor(0xB6091219); c.drawRoundRect(new RectF(18,18,310,112),18,18,p);
            p.setColor(0xFFD8BD79); p.setTextSize(18); p.setTypeface(Typeface.DEFAULT_BOLD); c.drawText("LORD OF THE NORTH",34,46,p);
            p.setColor(0xFF9EADB3); p.setTextSize(11); p.setTypeface(Typeface.DEFAULT); c.drawText("298 AC  •  THE NORTH REMEMBERS",34,67,p);
            p.setTextSize(10); c.drawText("VITALITY",34,88,p); c.drawText("RENOWN",176,88,p);
            p.setColor(0xFF9D3D42); c.drawRect(34,94,142,100,p); p.setColor(0xFFD8BD79); c.drawRect(176,94,258,100,p);
            p.setStyle(Paint.Style.FILL); p.setColor(0xC4071119); c.drawRoundRect(new RectF(w-330,18,w-18,118),18,18,p);
            p.setColor(0xFFD8BD79); p.setTextSize(15); p.setTypeface(Typeface.DEFAULT_BOLD); c.drawText("THE WOLF'S OATH",w-307,44,p);
            p.setColor(0xFFE1E7E9); p.setTextSize(11); p.setTypeface(Typeface.DEFAULT); drawWrap(c,game.renderer.quest,w-307,66,290,15);
            p.setColor(0xFF98A7AD); c.drawText("GOLD "+game.renderer.gold+"   RENOWN "+game.renderer.renown,w-307,108,p);
            // minimap
            RectF mm=new RectF(w-205,h-145,w-18,h-18); p.setColor(0xB8071119); c.drawRoundRect(mm,14,14,p);
            p.setColor(0xFF35494D); c.drawRect(mm.left+8,mm.top+8,mm.right-8,mm.bottom-8,p);
            p.setStrokeWidth(1); p.setColor(0xFF70817E); c.drawRect(mm.left+8,mm.top+8,mm.right-8,mm.bottom-8,p);
            drawMap(c,mm);
            // controls
            leftJoy.set(24,h-166,154,h-36); p.setStyle(Paint.Style.FILL); p.setColor(0x1AFFFFFF); c.drawCircle(89,h-101,65,p);
            p.setStyle(Paint.Style.STROKE); p.setStrokeWidth(2); p.setColor(0x45FFFFFF); c.drawCircle(89,h-101,65,p); p.setStyle(Paint.Style.FILL); p.setColor(0x58D8BD79); c.drawCircle(89+joyX*35,h-101+joyY*35,27,p);
            action.set(w-133,h-136,w-35,h-38); p.setColor(0xB51B2A32); c.drawOval(action,p); p.setStyle(Paint.Style.STROKE); p.setStrokeWidth(2); p.setColor(0x80D8BD79); c.drawOval(action,p); p.setStyle(Paint.Style.FILL); p.setColor(0xFFECE1C2); p.setTextSize(12); p.setTypeface(Typeface.DEFAULT_BOLD); c.drawText("INTERACT",w-117,h-84,p);
            p.setTypeface(Typeface.DEFAULT); p.setTextSize(10); p.setColor(0xFF9DADB3); c.drawText("DRAG RIGHT SIDE TO LOOK",w/2-65,h-18,p);
            if(dialog){
                p.setColor(0xE5091219); c.drawRoundRect(new RectF(38,h-190,w-38,h-38),20,20,p);
                p.setColor(0xFFD8BD79); p.setTextSize(17); p.setTypeface(Typeface.DEFAULT_BOLD); c.drawText(dialogueName,58,h-155,p);
                p.setColor(0xFFE3E8EA); p.setTypeface(Typeface.DEFAULT); p.setTextSize(12); drawWrap(c,dialogueText,58,h-130,w-116,18);
                p.setColor(0xFF2A3A42); c.drawRoundRect(new RectF(w-142,h-84,w-58,h-50),10,10,p); p.setColor(0xFFD8BD79); p.setTextSize(11); p.setTypeface(Typeface.DEFAULT_BOLD); c.drawText("CONTINUE",w-129,h-62,p);
            }
        }
        void drawWrap(Canvas c,String s,float x,float y,float maxW,float lh){ if(s==null)return; String[] words=s.split(" "); String line=""; float yy=y; for(String word:words){String t=line.isEmpty()?word:line+" "+word; if(p.measureText(t)>maxW && !line.isEmpty()){c.drawText(line,x,yy,p); yy+=lh; line=word;}else line=t;} if(!line.isEmpty())c.drawText(line,x,yy,p);}
        void drawMap(Canvas c,RectF r){
            float px=(game.renderer.playerX+220)/440f, pz=(game.renderer.playerZ+440)/880f;
            p.setColor(0xFFD8BD79); for(NorthRenderer.Site s:game.renderer.siteData){ float x=r.left+8+pxMap(s.x), y=r.top+8+pzMap(s.z); c.drawRect(x-2,y-2,x+2,y+2,p); }
            p.setColor(0xFFE05A5A); c.drawCircle(r.left+8+pxMap(game.renderer.playerX),r.top+8+pzMap(game.renderer.playerZ),4,p);
        }
        float pxMap(float x){return (x+220)/440f*171f;} float pzMap(float z){return (z+440)/880f*108f;}
        void interact(){ if(dialog){dialog=false; invalidate();return;} String[] d=game.renderer.interact(); if(d!=null){dialogueName=d[0];dialogueText=d[1];dialog=true;invalidate();} }
        @Override public boolean onTouchEvent(android.view.MotionEvent e){
            int a=e.getActionMasked(); float x=e.getX(),y=e.getY();
            if(a==MotionEvent.ACTION_DOWN){downX=x;downY=y;if(leftJoy.contains(x,y)){joyDown=true;updateJoy(x,y);}else if(action.contains(x,y)){interact();} game.renderer.touchDown(x,y); return true;}
            if(a==MotionEvent.ACTION_MOVE){if(joyDown)updateJoy(x,y); else if(downX>getWidth()*0.45f)game.renderer.touchLook(x-downX,y-downY);return true;}
            if(a==MotionEvent.ACTION_UP||a==MotionEvent.ACTION_CANCEL){if(joyDown){joyDown=false;joyX=joyY=0;game.renderer.setJoystick(0,0);invalidate();}game.renderer.touchUp();return true;} return true;
        }
        void updateJoy(float x,float y){float cx=89,cy=getHeight()-101;float dx=x-cx,dy=y-cy;float l=(float)Math.hypot(dx,dy);if(l>48){dx*=48/l;dy*=48/l;}joyX=dx/48f;joyY=dy/48f;game.renderer.setJoystick(joyX,joyY);invalidate();}
    }

    public static class NorthRenderer implements GLSurfaceView.Renderer {
        final Context ctx; final float[] proj=new float[16],view=new float[16],vp=new float[16],m=new float[16],tmp=new float[16];
        int program, posH,normH,uvH,mvpH,modelH,colH,lightH,texH,useTexH;
        volatile boolean glReady=false;
        Mesh cube, ground, cone; int texTerrain,texStone,texWood,texRock,texSnow,texForest;
        final ArrayList<NPC> npcs=new ArrayList<>(); final ArrayList<Site> siteData=new ArrayList<>();
        final Random rng=new Random(7); float playerX=0,playerZ=22,playerYaw=0,camYaw=0,camPitch=.36f; float joystickX,joystickY;
        int gold=100,renown=0; float stamina=100; String quest="Find Lord Eddard Stark in Winterfell."; long last; float dayTime=0.35f; int lastSite=-1;
        NorthRenderer(Context c){ctx=c;load();buildData();}
        void buildData(){
            addSite(0,0,"WINTERFELL","Seat of House Stark"); addSite(20,-110,"THE DREADFORT","House Bolton"); addSite(-105,-135,"WHITE HARBOR","House Manderly"); addSite(115,-145,"BEAR ISLAND","House Mormont"); addSite(-125,-250,"KARHOLD","House Karstark"); addSite(5,-260,"LAST HEARTH","House Umber"); addSite(125,-285,"DEEPWOOD MOTTE","House Glover"); addSite(-35,-350,"GREYWATER WATCH","House Reed"); addSite(65,-385,"THE WALL","The Night's Watch");
            npc("Eddard Stark","Stark","Lord of Winterfell",0,4,0x56636b); npc("Robb Stark","Stark","Heir to Winterfell",8,-3,0x445c68); npc("Jon Snow","Stark","The quiet son raised at Winterfell",-8,0,0x2d3941); npc("Bran Stark","Stark","Young Stark of Winterfell",4,9,0x5b7077); npc("Rickon Stark","Stark","Youngest Stark child",-5,10,0x60767d); npc("Sansa Stark","Stark","Lady of Winterfell",10,7,0x74707a); npc("Arya Stark","Stark","Young Stark daughter",-10,8,0x56666b);
            npc("Roose Bolton","Bolton","Lord of the Dreadfort",20,-110,0x68484d); npc("Ramsay Bolton","Bolton","Heir of the Dreadfort",28,-116,0x74464c);
            npc("Wyman Manderly","Manderly","Lord of White Harbor",-105,-135,0x5e7380); npc("Wylis Manderly","Manderly","Heir of White Harbor",-98,-142,0x5a7a83);
            npc("Maege Mormont","Mormont","Lady of Bear Island",115,-145,0x4b6261); npc("Alysane Mormont","Mormont","Daughter of Bear Island",123,-151,0x596f72); npc("Jeor Mormont","Mormont","Former lord of Bear Island",108,-157,0x646f72);
            npc("Rickard Karstark","Karstark","Lord of Karhold",-125,-250,0x667076); npc("Harrion Karstark","Karstark","Karstark heir",-118,-257,0x566976); npc("Greatjon Umber","Umber","Lord of Last Hearth",5,-260,0x636a6a); npc("Smalljon Umber","Umber","Son of the Greatjon",13,-266,0x566a72);
            npc("Galbart Glover","Glover","Lord of Deepwood Motte",125,-285,0x5f7368); npc("Howland Reed","Reed","Lord of Greywater Watch",-35,-350,0x536a5d); npc("Jojen Reed","Reed","Son of Howland Reed",-27,-356,0x627366); npc("Meera Reed","Reed","Daughter of Howland Reed",-43,-357,0x566f62);
        }
        void addSite(float x,float z,String name,String house){siteData.add(new Site(x,z,name,house));}
        void npc(String n,String h,String bio,float x,float z,int c){npcs.add(new NPC(n,h,bio,x,z,c));}

        @Override public void onSurfaceCreated(javax.microedition.khronos.opengles.GL10 gl, javax.microedition.khronos.egl.EGLConfig cfg){
            glReady=false;
            try{
                GLES20.glClearColor(.055f,.09f,.11f,1);
                GLES20.glEnable(GLES20.GL_DEPTH_TEST);
                GLES20.glEnable(GLES20.GL_CULL_FACE);
                GLES20.glCullFace(GLES20.GL_BACK);

                program=makeProgram(VS,FS);
                if(program==0) return;

                posH=GLES20.glGetAttribLocation(program,"aPos");
                normH=GLES20.glGetAttribLocation(program,"aNormal");
                uvH=GLES20.glGetAttribLocation(program,"aUV");
                mvpH=GLES20.glGetUniformLocation(program,"uMVP");
                modelH=GLES20.glGetUniformLocation(program,"uModel");
                colH=GLES20.glGetUniformLocation(program,"uColor");
                lightH=GLES20.glGetUniformLocation(program,"uLight");
                texH=GLES20.glGetUniformLocation(program,"uTex");
                useTexH=GLES20.glGetUniformLocation(program,"uUseTex");

                if(posH<0 || normH<0 || uvH<0 || mvpH<0 || modelH<0 || colH<0 || lightH<0 || texH<0 || useTexH<0) return;

                cube=meshCube();
                cone=meshCone();
                ground=meshGround();
                if(cube==null || cone==null || ground==null) return;

                texStone=loadTex("castle_stone.jpg");
                texWood=loadTex("wood_bark.jpg");
                texRock=loadTex("wet_rock.jpg");
                texSnow=loadTex("frozen_ground.jpg");
                texForest=loadTex("pine_forest.jpg");

                last=System.nanoTime();
                glReady=true;
            }catch(Throwable ignored){
                glReady=false;
            }
        }
        @Override public void onSurfaceChanged(javax.microedition.khronos.opengles.GL10 gl,int w,int h){
            if(w<=0 || h<=0) return;
            GLES20.glViewport(0,0,w,h);
            Matrix.perspectiveM(proj,0,63,(float)w/h,.1f,700f);
        }
        @Override public void onDrawFrame(javax.microedition.khronos.opengles.GL10 gl){ if(program==0) return; long now=System.nanoTime();float dt=Math.min(.04f,(now-last)/1e9f);last=now;update(dt);render(); }
        void update(float dt){
            dayTime=(dayTime+dt*.006f)%1f;
            float fx=-joystickY, rx=joystickX; float len=(float)Math.hypot(fx,rx); if(len>1){fx/=len;rx/=len;}
            float speed=6.2f; if(stamina>0 && Math.abs(fx)+Math.abs(rx)>1.4){speed=9.0f; stamina=Math.max(0,stamina-dt*18);}else stamina=Math.min(100,stamina+dt*10);
            float sy=(float)Math.sin(playerYaw),cy=(float)Math.cos(playerYaw);
            playerX += (rx*cy+fx*sy)*speed*dt; playerZ += (fx*cy-rx*sy)*speed*dt;
            playerX=Math.max(-198,Math.min(198,playerX));playerZ=Math.max(-410,Math.min(80,playerZ));
            // discover sites / progress quest
            int near=-1; float bd=999; for(int i=0;i<siteData.size();i++){Site s=siteData.get(i);float d=(float)Math.hypot(playerX-s.x,playerZ-s.z);if(d<bd){bd=d;near=i;}}
            if(near>=0 && bd<20 && near!=lastSite){lastSite=near; if(near==8)quest="Reach the Wall and speak to the Night's Watch."; else quest="Explore "+siteData.get(near).name+" and speak with its northern lord."; if(near==0)quest="Find Lord Eddard Stark in Winterfell.";}
        }
        void render(){
            float daylight=(float)(.55+.45*Math.sin(dayTime*Math.PI*2)); GLES20.glClearColor(.035f+.12f*daylight,.065f+.14f*daylight,.08f+.16f*daylight,1); GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT|GLES20.GL_DEPTH_BUFFER_BIT);
            float cx=playerX-(float)Math.sin(camYaw)*12f, cz=playerZ-(float)Math.cos(camYaw)*12f, cy=5.4f+camPitch*3;
            Matrix.setLookAtM(view,0,cx,cy,cz,playerX,1.0f,playerZ,0,1,0); Matrix.multiplyMM(vp,0,proj,0,view,0);
            GLES20.glUseProgram(program); GLES20.glUniform3f(lightH,-.35f,.9f,.45f);
            drawGround(daylight); drawWorld(); drawPlayer(); drawSnow();
        }
        void drawGround(float d){drawMesh(ground,0,0,0,440,1,900,texSnow,new float[]{.75f+.15f*d,.82f+.12f*d,.82f+.12f*d,1});}
        void drawWorld(){
            // roads
            for(int i=0;i<siteData.size();i++){Site s=siteData.get(i); drawBuilding(s.x,0,s.z,10,12,10,texStone); for(int k=0;k<7;k++){float ax=s.x+(k-3)*3.5f, az=s.z+15; drawMesh(cube,ax,.08f,az,3.1f,.16f,3.1f,texRock,new float[]{.7f,.72f,.71f,1});}}
            // forests around the North
            for(int i=0;i<30;i++){float a=(float)Math.sin(i*91.7)*.5f+.5f;float b=(float)Math.cos(i*37.1)*.5f+.5f;float x=(a*396)-198,z=(b*480)-400; if(Math.hypot(x,z-22)<30)continue; drawTree(x,z,.75f+.65f*(float)((i*17)%10)/10f);}
            // rocks / hills
            for(int i=0;i<15;i++){float x=(float)Math.sin(i*14.31)*190,z=-45-(float)Math.cos(i*11.19)*360;drawMesh(cone,x,0,z,1.2f+(i%5),1.3f+(i%4)*.6f,1.2f+(i%5),texRock,new float[]{.65f,.68f,.67f,1});}
            for(NPC n:npcs)drawNPC(n);
            // wall monument
            for(int k=0;k<10;k++) drawMesh(cube,65+k*9,10,-395,7,24,8,texStone,new float[]{.76f,.78f,.79f,1});
        }
        void drawBuilding(float x,float y,float z,float sx,float sy,float sz,int tex){
            drawMesh(cube,x,y+sy*.5f,z,sx,sy,sz,tex,new float[]{.92f,.92f,.91f,1});
            for(int i=0;i<4;i++){double a=i*Math.PI*.5;float tx=x+(float)Math.cos(a)*(sx*.52f),tz=z+(float)Math.sin(a)*(sz*.52f);drawMesh(cube,tx,y+sy+2,tz,3,sy*.65f,3,tex,new float[]{.78f,.79f,.80f,1});}
        }
        void drawTree(float x,float z,float s){drawMesh(cube,x,2.1f*s,z,.6f*s,4.2f*s,.6f*s,texWood,new float[]{.86f,.75f,.64f,1});drawMesh(cone,x,6.2f*s,z,3.2f*s,7.0f*s,3.2f*s,texForest,new float[]{.7f,.86f,.74f,1});}
        void drawNPC(NPC n){
            float bob=(float)Math.sin((System.nanoTime()/1e8+n.x*2))*0.015f; float s=1;
            drawMesh(cube,n.x,1.1f+bob,n.z,.75f,1.55f,.48f,texWood,color(n.color)); drawMesh(cube,n.x,2.15f+bob,n.z,.64f,.64f,.64f,texSnow,new float[]{.95f,.78f,.68f,1});
            drawMesh(cube,n.x,1.35f+bob,n.z-.35f,.9f,1.1f,.12f,texForest,new float[]{.78f,.78f,.78f,1});
        }
        void drawPlayer(){drawMesh(cube,playerX,1.15f,playerZ,.8f,1.7f,.52f,texWood,new float[]{.74f,.77f,.80f,1});drawMesh(cube,playerX,2.28f,playerZ,.66f,.66f,.66f,texSnow,new float[]{.98f,.80f,.70f,1});drawMesh(cube,playerX,1.45f,playerZ-.38f,.98f,1.18f,.12f,texForest,new float[]{.9f,.9f,.9f,1});}
        void drawSnow(){/* visual atmosphere is handled by animated world tint; avoids expensive particles on low-end phones */}
        float[] color(int c){return new float[]{((c>>16)&255)/255f,((c>>8)&255)/255f,(c&255)/255f,1};}
        void drawMesh(Mesh mesh,float x,float y,float z,float sx,float sy,float sz,int tex,float[] col){
            if(!glReady || mesh==null || program==0 || posH<0 || normH<0 || uvH<0) return;
            Matrix.setIdentityM(m,0);Matrix.translateM(m,0,x,y,z);Matrix.scaleM(m,0,sx,sy,sz);Matrix.multiplyMM(tmp,0,vp,0,m,0);GLES20.glUniformMatrix4fv(mvpH,1,false,tmp,0);GLES20.glUniformMatrix4fv(modelH,1,false,m,0);GLES20.glUniform4fv(colH,1,col,0);GLES20.glUniform1i(useTexH,tex>0?1:0);if(tex>0){GLES20.glActiveTexture(GLES20.GL_TEXTURE0);GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,tex);GLES20.glUniform1i(texH,0);}mesh.bind(posH,normH,uvH);mesh.ib.position(0);
            GLES20.glDrawElements(GLES20.GL_TRIANGLES,mesh.count,GLES20.GL_UNSIGNED_SHORT,mesh.ib);
            mesh.ib.position(0);mesh.unbind(posH,normH,uvH);}
        String[] interact(){NPC best=null;float bd=4.2f;for(NPC n:npcs){float d=(float)Math.hypot(playerX-n.x,playerZ-n.z);if(d<bd){bd=d;best=n;}}if(best==null)return null;renown+=5; if(best.name.equals("Eddard Stark"))quest="Visit the northern lords and strengthen your standing."; if(best.name.equals("Howland Reed"))quest="Reach the Wall and speak with the Night's Watch."; save(); return new String[]{best.name+"  •  House "+best.house,dialogLine(best.name)};}
        String dialogLine(String name){switch(name){case "Eddard Stark":return "The North is held by oaths, old roads, and the people who keep both.";case "Robb Stark":return "A pack moves farther than a lone wolf. Remember your allies.";case "Jon Snow":return "The cold teaches you to watch the horizon and listen before you act.";case "Wyman Manderly":return "White Harbor remembers every friend who arrives at its gates.";case "Maege Mormont":return "Bear Island stands stubborn beneath a northern sky.";case "Roose Bolton":return "The North has many banners. Know where each one stands.";case "Greatjon Umber":return "Last Hearth keeps a long memory and a loud hall.";case "Howland Reed":return "The Neck hides more paths than any map can show.";default:return "The North remembers. Every lord and village has a place in its story.";}}
        void setJoystick(float x,float y){joystickX=x;joystickY=y;} void touchDown(float x,float y){} void touchUp(){} void touchLook(float dx,float dy){if(Math.abs(dx)+Math.abs(dy)<1)return;camYaw-=dx*.0042f;camPitch=Math.max(.15f,Math.min(.62f,camPitch-dy*.0022f));}
        void save(){ctx.getSharedPreferences("north_save",Context.MODE_PRIVATE).edit().putInt("renown",renown).putInt("gold",gold).putFloat("x",playerX).putFloat("z",playerZ).apply();}
        void load(){android.content.SharedPreferences p=ctx.getSharedPreferences("north_save",Context.MODE_PRIVATE);renown=p.getInt("renown",0);gold=p.getInt("gold",100);playerX=p.getFloat("x",0);playerZ=p.getFloat("z",22);}
        static int makeProgram(String vs,String fs){
            int v=compile(GLES20.GL_VERTEX_SHADER,vs);
            int f=compile(GLES20.GL_FRAGMENT_SHADER,fs);
            if(v==0 || f==0){ if(v!=0) GLES20.glDeleteShader(v); if(f!=0) GLES20.glDeleteShader(f); return 0; }
            int p=GLES20.glCreateProgram();
            if(p==0) return 0;
            GLES20.glAttachShader(p,v); GLES20.glAttachShader(p,f); GLES20.glLinkProgram(p);
            int[] ok={0}; GLES20.glGetProgramiv(p,GLES20.GL_LINK_STATUS,ok,0);
            GLES20.glDeleteShader(v); GLES20.glDeleteShader(f);
            if(ok[0]==0){ GLES20.glDeleteProgram(p); return 0; }
            return p;
        }
        static int compile(int type,String src){
            int s=GLES20.glCreateShader(type);
            if(s==0) return 0;
            GLES20.glShaderSource(s,src); GLES20.glCompileShader(s);
            int[] ok={0}; GLES20.glGetShaderiv(s,GLES20.GL_COMPILE_STATUS,ok,0);
            if(ok[0]==0){ GLES20.glDeleteShader(s); return 0; }
            return s;
        }
        int loadTex(String name){
            android.graphics.Bitmap b=null;
            try{
                android.content.res.AssetManager am=ctx.getAssets();
                android.graphics.BitmapFactory.Options o=new android.graphics.BitmapFactory.Options();
                o.inPreferredConfig=android.graphics.Bitmap.Config.RGB_565;
                o.inScaled=false;
                try(java.io.InputStream in1=am.open(name)){
                    android.graphics.BitmapFactory.Options bounds=new android.graphics.BitmapFactory.Options(); bounds.inJustDecodeBounds=true;
                    android.graphics.BitmapFactory.decodeStream(in1,null,bounds);
                    int max=Math.max(bounds.outWidth,bounds.outHeight);
                    o.inSampleSize=1;
                    while(max/o.inSampleSize>512) o.inSampleSize*=2;
                }
                try(java.io.InputStream in2=am.open(name)){ b=android.graphics.BitmapFactory.decodeStream(in2,null,o); }
                if(b==null) return 0;
                int[] t={0}; GLES20.glGenTextures(1,t,0); if(t[0]==0) return 0;
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,t[0]);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_MIN_FILTER,GLES20.GL_LINEAR);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_MAG_FILTER,GLES20.GL_LINEAR);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_WRAP_S,GLES20.GL_CLAMP_TO_EDGE);
                GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D,GLES20.GL_TEXTURE_WRAP_T,GLES20.GL_CLAMP_TO_EDGE);
                android.opengl.GLUtils.texImage2D(GLES20.GL_TEXTURE_2D,0,b,0);
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D,0);
                return t[0];
            }catch(Throwable ignored){ return 0; }
            finally{ if(b!=null && !b.isRecycled()) b.recycle(); }
        }
        Mesh meshCube(){float[] v={-1,-1,-1, 1,-1,-1,1,1,-1,-1,1,-1,-1,-1,1,1,-1,1,1,1,1,-1,1,1};float[][] uv={{0,0},{1,0},{1,1},{0,1}};short[] idx={0,1,2,2,3,0,1,5,6,6,2,1,5,4,7,7,6,5,4,0,3,3,7,4,3,2,6,6,7,3,4,5,1,1,0,4};float[] verts=new float[24*8];int k=0;float[][] ns={{0,0,-1},{1,0,0},{0,0,1},{-1,0,0},{0,1,0},{0,-1,0}};int[][] faces={{0,1,2,3},{1,5,6,2},{5,4,7,6},{4,0,3,7},{3,2,6,7},{4,5,1,0}};float[][] corners={{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},{-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}};for(int f=0;f<6;f++)for(int q=0;q<4;q++){float[] c=corners[faces[f][q]];verts[k++]=c[0];verts[k++]=c[1];verts[k++]=c[2];verts[k++]=ns[f][0];verts[k++]=ns[f][1];verts[k++]=ns[f][2];verts[k++]=uv[q][0];verts[k++]=uv[q][1];}short[] id=new short[36];int p=0;for(short f=0;f<6;f++){short b=(short)(f*4);id[p++]=(short)(b);id[p++]=(short)(b+1);id[p++]=(short)(b+2);id[p++]=(short)(b+2);id[p++]=(short)(b+3);id[p++]=(short)(b);}return new Mesh(verts,id);}
        Mesh meshCone(){int N=12;float[] a=new float[(N+1)*8*2];short[] idx=new short[N*6];int k=0,p=0;for(int i=0;i<N;i++){double aa=i*2*Math.PI/N,bb=(i+1)*2*Math.PI/N;float x1=(float)Math.cos(aa),z1=(float)Math.sin(aa),x2=(float)Math.cos(bb),z2=(float)Math.sin(bb);float[] quad={x1,-1,z1,x1,0,z1,.0f,0,x2,-1,z2,x2,0,z2,1,0,x2,1,z2,x2,0,z2,1,1,x1,1,z1,x1,0,z1,0,1};System.arraycopy(quad,0,a,k,quad.length>0?0:0);k+=quad.length;}/* fallback: use cube-like cone through parametric helper */ return meshPyramid();}
        Mesh meshPyramid(){int N=8;ArrayList<Float> v=new ArrayList<>();ArrayList<Short> id=new ArrayList<>();for(int i=0;i<N;i++){double a=i*2*Math.PI/N,b=(i+1)*2*Math.PI/N;float x1=(float)Math.cos(a),z1=(float)Math.sin(a),x2=(float)Math.cos(b),z2=(float)Math.sin(b);int base=v.size()/8;put(v,x1,-1,z1,0,.7f,0,0,0);put(v,x2,-1,z2,0,.7f,0,1,0);put(v,0,1,0,0,.7f,0,.5f,1);id.add((short)base);id.add((short)(base+1));id.add((short)(base+2));}float[] va=new float[v.size()];for(int i=0;i<v.size();i++)va[i]=v.get(i);short[] ia=new short[id.size()];for(int i=0;i<id.size();i++)ia[i]=id.get(i);return new Mesh(va,ia);}
        void put(ArrayList<Float> v,float x,float y,float z,float nx,float ny,float nz,float u,float vv){v.add(x);v.add(y);v.add(z);v.add(nx);v.add(ny);v.add(nz);v.add(u);v.add(vv);}
        Mesh meshGround(){int N=72;float size=440;ArrayList<Float> v=new ArrayList<>();ArrayList<Short> id=new ArrayList<>();for(int z=0;z<=N;z++)for(int x=0;x<=N;x++){float px=-size/2+size*x/N,pz=-size/2+size*z/N;float h=(float)(2.2*Math.sin(px*.018)*Math.cos(pz*.014)+1.2*Math.sin((px+pz)*.035));put(v,px,h-3,pz,0,1,0,px/44f,pz/44f);}for(int z=0;z<N;z++)for(int x=0;x<N;x++){int a=z*(N+1)+x,b=a+1,c=a+(N+1),d=c+1;id.add((short)a);id.add((short)c);id.add((short)b);id.add((short)b);id.add((short)c);id.add((short)d);}float[] va=new float[v.size()];for(int i=0;i<v.size();i++)va[i]=v.get(i);short[] ia=new short[id.size()];for(int i=0;i<id.size();i++)ia[i]=id.get(i);return new Mesh(va,ia);}
        static class Mesh{FloatBuffer vb;ShortBuffer ib;int count;Mesh(float[] v,short[] i){vb=ByteBuffer.allocateDirect(v.length*4).order(ByteOrder.nativeOrder()).asFloatBuffer();vb.put(v).position(0);ib=ByteBuffer.allocateDirect(i.length*2).order(ByteOrder.nativeOrder()).asShortBuffer();ib.put(i).position(0);count=i.length;}void bind(int p,int n,int u){vb.position(0);GLES20.glEnableVertexAttribArray(p);GLES20.glVertexAttribPointer(p,3,GLES20.GL_FLOAT,false,32,vb);vb.position(3);GLES20.glEnableVertexAttribArray(n);GLES20.glVertexAttribPointer(n,3,GLES20.GL_FLOAT,false,32,vb);vb.position(6);GLES20.glEnableVertexAttribArray(u);GLES20.glVertexAttribPointer(u,2,GLES20.GL_FLOAT,false,32,vb);}void unbind(int p,int n,int u){GLES20.glDisableVertexAttribArray(p);GLES20.glDisableVertexAttribArray(n);GLES20.glDisableVertexAttribArray(u);}}
        static class NPC{String name,house,bio;float x,z;int color;NPC(String n,String h,String b,float X,float Z,int c){name=n;house=h;bio=b;x=X;z=Z;color=c;}}
        static class Site{float x,z;String name,house;Site(float X,float Z,String N,String H){x=X;z=Z;name=N;house=H;}}
        static final String VS="attribute vec3 aPos; attribute vec3 aNormal; attribute vec2 aUV; uniform mat4 uMVP; uniform mat4 uModel; varying vec3 vN; varying vec2 vUV; void main(){vN=aNormal;vUV=aUV;gl_Position=uMVP*vec4(aPos,1.0);}";
        static final String FS="precision mediump float; varying vec3 vN; varying vec2 vUV; uniform vec4 uColor; uniform vec3 uLight; uniform sampler2D uTex; uniform int uUseTex; void main(){float d=max(dot(normalize(vN),normalize(uLight)),0.0);vec4 base=uColor;if(uUseTex==1)base*=texture2D(uTex,vUV);vec3 outc=base.rgb*(0.32+0.68*d);gl_FragColor=vec4(outc,1.0);} ";
    }
}
