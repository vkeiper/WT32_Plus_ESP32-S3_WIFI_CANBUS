#include <ArduinoJson.h>
DynamicJsonDocument doc(3072);

void doCreateJsonDoc(void){

    JsonObject panel_0 = doc.createNestedObject("panel_0");
    panel_0["headerTxt"] = "Control";
    panel_0["values"] = "1";
    panel_0["units"] = "Address";
    
    JsonArray panel_0_limits = panel_0.createNestedArray("limits");
    panel_0_limits.add(1);
    panel_0_limits.add(4);
    
    JsonArray panel_0_colors = panel_0.createNestedArray("colors");
    panel_0_colors.add("GREEN");
    panel_0_colors.add("RED");
    panel_0_colors.add("GREY");
    panel_0["fontsz"] = 10;
    
    JsonObject panel_1 = doc.createNestedObject("panel_1");
    panel_1["headerTxt"] = "Output Status";
    
    JsonArray panel_1_values = panel_1.createNestedArray("values");
    panel_1_values.add(30.5);
    panel_1_values.add(36.5);
    panel_1_values.add("ON");
    
    JsonArray panel_1_units = panel_1.createNestedArray("units");
    panel_1_units.add("Volts DC");
    panel_1_units.add("Amps DC");
    panel_1_units.add("Output ENA");
    
    JsonArray panel_1_limits = panel_1.createNestedArray("limits");
    
    JsonArray panel_1_limits_0 = panel_1_limits.createNestedArray();
    panel_1_limits_0.add(26.5);
    panel_1_limits_0.add(36.6);
    
    JsonArray panel_1_limits_1 = panel_1_limits.createNestedArray();
    panel_1_limits_1.add(0);
    panel_1_limits_1.add(100);
    
    JsonArray panel_1_limits_2 = panel_1_limits.createNestedArray();
    panel_1_limits_2.add("OFF");
    panel_1_limits_2.add("ON");
    
    JsonArray panel_1_colors = panel_1.createNestedArray("colors");
    panel_1_colors.add("YELLOW");
    panel_1_colors.add("GREEN");
    panel_1_colors.add("RED");
    panel_1["fontsz"] = 10;
    
    JsonObject panel_2 = doc.createNestedObject("panel_2");
    panel_2["headerTxt"] = "System Reference Electrodes";
    
    JsonArray panel_2_values = panel_2.createNestedArray("values");
    panel_2_values.add(200.1);
    panel_2_values.add(150.2);
    panel_2_values.add(142.01);
    panel_2_values.add(202.2);
    
    JsonArray panel_2_units = panel_2.createNestedArray("units");
    panel_2_units.add("mV");
    panel_2_units.add("mV");
    panel_2_units.add("mV");
    panel_2_units.add("mV");
    
    JsonArray panel_2_limits = panel_2.createNestedArray("limits");
    
    JsonArray panel_2_limits_0 = panel_2_limits.createNestedArray();
    panel_2_limits_0.add(-200.2);
    panel_2_limits_0.add(200);
    
    JsonArray panel_2_limits_1 = panel_2_limits.createNestedArray();
    panel_2_limits_1.add(-201);
    panel_2_limits_1.add(201);
    
    JsonArray panel_2_limits_2 = panel_2_limits.createNestedArray();
    panel_2_limits_2.add(-202);
    panel_2_limits_2.add(202);
    
    JsonArray panel_2_limits_3 = panel_2_limits.createNestedArray();
    panel_2_limits_3.add(-203);
    panel_2_limits_3.add(203);
    
    JsonArray panel_2_colors = panel_2.createNestedArray("colors");
    panel_2_colors.add("GREY");
    panel_2_colors.add("BLUE");
    panel_2_colors.add("RED");
    panel_2["fontsz"] = 10;
    
    JsonObject panel_3 = doc.createNestedObject("panel_3");
    panel_3["headerTxt"] = "Anode Current Monitors";
    
    JsonArray panel_3_values = panel_3.createNestedArray("values");
    panel_3_values.add(100);
    panel_3_values.add(100.1);
    panel_3_values.add(100.2);
    panel_3_values.add(100.3);
    panel_3_values.add(100.4);
    panel_3_values.add(100.5);
    
    JsonArray panel_3_units = panel_3.createNestedArray("units");
    panel_3_units.add("A");
    panel_3_units.add("A");
    panel_3_units.add("A");
    panel_3_units.add("A");
    panel_3_units.add("A");
    panel_3_units.add("A");
    
    JsonArray panel_3_limits = panel_3.createNestedArray("limits");
    
    JsonArray panel_3_limits_0 = panel_3_limits.createNestedArray();
    panel_3_limits_0.add(80);
    panel_3_limits_0.add(130);
    
    JsonArray panel_3_limits_1 = panel_3_limits.createNestedArray();
    panel_3_limits_1.add(80.1);
    panel_3_limits_1.add(130.1);
    
    JsonArray panel_3_limits_2 = panel_3_limits.createNestedArray();
    panel_3_limits_2.add(80.2);
    panel_3_limits_2.add(130.2);
    
    JsonArray panel_3_limits_3 = panel_3_limits.createNestedArray();
    panel_3_limits_3.add(80.3);
    panel_3_limits_3.add(130.3);
    
    JsonArray panel_3_limits_4 = panel_3_limits.createNestedArray();
    panel_3_limits_4.add(80.4);
    panel_3_limits_4.add(130.4);
    
    JsonArray panel_3_limits_5 = panel_3_limits.createNestedArray();
    panel_3_limits_5.add(80.5);
    panel_3_limits_5.add(130.5);
    
    JsonArray panel_3_colors = panel_3.createNestedArray("colors");
    panel_3_colors.add("GREY");
    panel_3_colors.add("BLUE");
    panel_3_colors.add("RED");
    panel_3["fontsz"] = 10;
    
    JsonObject panel_4 = doc.createNestedObject("panel_4");
    panel_4["headerTxt"] = "Assert Faults";
    panel_4["values"] = "PASSING";
    panel_4["units"] = "NA";
    
    JsonArray panel_4_limits = panel_4.createNestedArray("limits");
    panel_4_limits.add(65535);
    panel_4_limits.add(65535);
    
    JsonArray panel_4_colors = panel_4.createNestedArray("colors");
    panel_4_colors.add("YELLOW");
    panel_4_colors.add("GREEN");
    panel_4_colors.add("RED");
    panel_4["fontsz"] = 10;

}    
