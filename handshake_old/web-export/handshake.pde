Table AQData;
import processing.serial.*;

Serial myPort;  // Create object from Serial class
int tableRows;
int rowCount = 0;
String timestamp;
float pm2_5;
float CO; 
String value;
boolean firstContact = false;
String val;

void setup() {
  size(200,200); //make our canvas 200 x 200 pixels big
  String portName = Serial.list()[1]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
  AQData = loadTable("WOEIP_07_07_15_peak2.csv", "header");
  tableRows = AQData.getRowCount();
  myPort.bufferUntil('\n');
  /*print table to status bar

  println(AQData.getRowCount() + " total rows in table"); 

  for (TableRow row : AQData.rows()) {
    
    String timestamp = row.getString("timestamp");
    float pm2_5 = row.getFloat("PM 2.5");
    float CO = row.getFloat("CO");
    
    println(timestamp +  "has a PM2.5 of " + pm2_5 + " and a CO of " + CO);
  }*/
  
}

void draw(){
}
void serialEvent(Serial myPort)
{
  val = myPort.readStringUntil('\n');
  if (val != null){
    val = trim(val);
    println(val);
  }
  if (firstContact == false){
    if (val.equals("A")){
      myPort.clear();
      firstContact = true;
      myPort.write("A");
      //println("contact");

    }
  }
  else{ 
    if (rowCount < tableRows){
      TableRow row = AQData.getRow(rowCount);
      timestamp = row.getString("Timestamp");
//      pm2_5 = row.getFloat("PM 2.5");
      CO = row.getFloat("Value"); 
      value = String.format("%.2f",CO);
      rowCount++;
      value += '\n';
      for (int i = 0; i < value.length(); i++){
        myPort.write(value.charAt(i));
        
      }
    }
  }
}


