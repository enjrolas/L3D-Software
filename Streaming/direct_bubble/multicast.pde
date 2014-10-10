
// import UDP library
import hypermedia.net.*;
UDP udp;  // define the UDP object
boolean direct;
String address;

void initMulticast(int port)
{
  direct=false;
  // create a new datagram connection on port 6000
  // and wait for incomming message
  // create a multicast connection on port 6000
  // and join the group at the address "224.0.0.1"
  String ip       = "224.0.0.1";  // the remote IP address
  udp = new UDP( this, port, ip );
  // ... well, just verifies if it's really a multicast socket and blablabla
  println( "init as multicast socket ... "+udp.isMulticast() );
  println( "joins a multicast group  ... "+udp.isJoined() );
  //udp.log( true );     // <-- printout the connection activity
}

void initDirectStreaming(String ip, int port)
{
  direct=true;
  udp = new UDP( this, port, ip );
  // ... well, just verifies if it's really a multicast socket and blablabla
  println( "init as multicast socket ... "+udp.isMulticast() );
  println( "joins a multicast group  ... "+udp.isJoined() );
  //udp.log( true );     // <-- printout the connection activity
}

void initDirectStreaming(String ip)
{
  direct=true;
  address=ip;
  udp = new UDP( this );
  // ... well, just verifies if it's really a multicast socket and blablabla
  println( "init as multicast socket ... "+udp.isMulticast() );
  println( "joins a multicast group  ... "+udp.isJoined() );
  //udp.log( true );     // <-- printout the connection activity
}


void sendData()
{
  int index=0;
  byte[] data=new byte[(int)pow(side, 3)];
  for (int z=0; z<side; z++)
    for (int y=0; y<side; y++)
      for (int x=0; x<side; x++)
      {
        index=z*64+y*8+x;
        data[index]=colorByte(cube[x][y][z]);
      }
  if (direct)
    udp.send( data, address, port);
  else
    udp.send( data);
}

byte colorByte(color col)
{
  return (byte)((int)red(col)&224 | ((int)green(col)&224)>>3 | ((int)blue(col)&192)>>6);
}

