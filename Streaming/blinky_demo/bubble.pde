
class Bubble {
  float radius;
  PVector center;
  float popRadius;
  color bubbleColor;
  float inc;
  boolean popped, gone;
  Bubble()
  {
    center=new PVector(random(side), -4, random(side));
    radius=random(0,2);
    popRadius=random(side/2);
    //random color
    //bubbleColor=colorMap(random(1000), 0, 1000);
    bubbleColor=color(0,50,100,100);
    inc=random(0.02, 0.075);
    popped=false;
    gone=false;
  }
  Bubble(int _x, int _y, int _z, int rad, color col)
  {
    center=new PVector(_x, _y, _z);
    radius=rad;
    //random color
    //bubbleColor=colorMap(random(1000), 0, 1000);
    bubbleColor=col;
  }
  void update()
  {
    if(center.y>12)
      gone=true;
    if(popped)
    {
      bubbleColor=color(128,0,0,122);
      radius+=inc;
      if(radius>side/2)
        gone=true;
    }
    else
      center.y+=inc;
}
  void drawBubble()
  {
      drawSphere(center, radius, bubbleColor);
  }

  void drawPoint()
  {
      setVoxel((int)center.x, (int)center.y, (int)center.z, bubbleColor);
  }

  void drawSphere(PVector pos, float radius, color col)
  {
    float res=30;
    for (float m=0; m<res; m++)
      for (float n=0; n<res; n++)
        setVoxel(round(pos.x+radius*sin(PI * m/res)*cos(2*PI * n/res)), round(pos.y + radius*sin(PI * m/res)*sin(2*PI * n/res)), round(pos.z + radius*cos(PI * m/res)), col);
  }
}


