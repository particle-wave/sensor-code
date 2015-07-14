Table aqdata;

void setup() {
  
  table = loadTable("Louisville03.csv", "header");

  println(table.getRowCount() + " total rows in table"); 

  for (TableRow row : table.rows()) {
    
    int id = row.getInt("timestamp");
    float pm2_5 = row.getFloat("species");
    float CO = row.getFloat("CO");
    
    println(id + " (" + timestamp + ") has a PM2.5 of " + pm2_5 + "and a CO of" + CO);
    
  }
  
}

void draw()
{
}

/*
import processing.serial.*;

Serial myPort;  // Create object from Serial class

void setup() 
{
  size(200,200); //make our canvas 200 x 200 pixels big
  String portName = Serial.list()[0]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
}

void draw() {
  if (mousePressed == true) 
  {                           //if we clicked in the window
   myPort.write('1');         //send a 1
   println("1");   
  } else 
  {                           //otherwise
  myPort.write('0');          //send a 0
  }   
}
*/

