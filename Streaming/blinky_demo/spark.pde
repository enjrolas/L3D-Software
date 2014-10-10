import java.util.Iterator;
JSONArray myCores=new JSONArray();

void loadCores(String accessToken)
{
  JSONArray cores;
  cores=loadJSONArray("https://api.spark.io/v1/devices/?access_token="+accessToken);  //general URL for my account


  //println(cores);

  for (int i=0;i<cores.size();i++)
  {
    try {
      JSONObject core=cores.getJSONObject(i);
      String lastHeard=core.getString("last_heard");
      if (lastHeard!=null)
      {
        String deviceID=core.getString("id");
        String deviceName=core.getString("name");
        //println("last heard from device "+deviceName+" ("+deviceID+") on "+lastHeard);
        String deviceURL="https://api.spark.io/v1/devices/" + deviceID + "/?access_token=" + accessToken;
        //println(deviceURL);
        JSONObject variables=loadJSONObject(deviceURL);
        //println(variables);
        JSONObject activeVariables=variables.getJSONObject("variables");
        //println(activeVariables.keys());
        JSONObject currentCore=new JSONObject();
        myCores.setJSONObject(i, currentCore);
        currentCore.setString("name", deviceName);
        currentCore.setString("deviceID", deviceID);
        for (Object variableName : activeVariables.keys())
        {
          //println(variableName);
          String variableURL="https://api.spark.io/v1/devices/" + deviceID +"/"+ variableName + "/?access_token=" + accessToken;
          JSONObject variable=loadJSONObject(variableURL);
          //println(variable);
          String value=variable.getString("result");
          //println(value);
          currentCore.setString((String)variableName, value);
        }
      }
    }
    catch(Exception e) {
      //    println(e);
      //    println("something went wrong there...");
    }
  }
  println(myCores);
}

String getVar(String coreName, String varName)
{
  boolean varFound=false;
  for (int i=0;((i<myCores.size())&&(!varFound));i++)
  {
    JSONObject core=myCores.getJSONObject(i);
    if (core.getString("name").equals(coreName))
    try {
      return(core.getString(varName));
    }
    catch(Exception e) //variable name not found
    {
      return null;
    }
  }

  return null;
}

