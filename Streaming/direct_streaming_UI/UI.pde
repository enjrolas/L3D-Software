import controlP5.*;

//UI variables
ControlP5 cp5;
String[] names;
RadioButton r;

void initUI()
{
  cp5 = new ControlP5(this);
  cp5.setAutoDraw(false);  
  initStreaming();
  names=getNames();

  cp5.addTextfield("accessTokenField")
     .setPosition(20,400)
     .setSize(250,19)
     .setFocus(true)
     .setColor(color(255,0,0))
     .setText(accessToken)
     ;
     
  r = cp5.addRadioButton("coreSelector")
    .setPosition(50, 200)
      .setSize(40, 20)
        .setColorForeground(color(120))
          .setColorActive(color(255))
            .setColorLabel(color(255))
              .setItemsPerRow(1)
                .setSpacingColumn(50);

  for (int i=0;i<names.length;i++)
    r.addItem(names[i], i);
  if (names.length>0)
    streamToCore(names[0]);

  for (Toggle t:r.getItems()) {
    t.captionLabel().setColorBackground(color(255, 80));
    t.captionLabel().style().moveMargin(-7, 0, 0, -3);
    t.captionLabel().style().movePadding(7, 0, 0, 3);
    t.captionLabel().style().backgroundWidth = 45;
    t.captionLabel().style().backgroundHeight = 13;
  }
  
}

void streamToCore(String name)
{
  int index=nameIndex(name);
  r.activate(index);
  coreName=name;
  initDirectStreaming(name);
}

//returns the numerical index from the global array 'names' for the String matching the 'name' parameter
//returns -1 if no match is found
int nameIndex(String name)
{
  for (int i=0;i<names.length;i++)
    if (names[i].equals(name))
      return(i);
  return -1;
}

void coreSelector(int index) {
  println("selecting core number "+index);
  String name=names[index];
  println("switching to core: "+name);
  streamToCore(name);
}

public void accessTokenField(String token) {
  accessToken=token;
  initUI();
}

