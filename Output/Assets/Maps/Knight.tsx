<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.2" tiledversion="1.2.4" name="Knight" tilewidth="58" tileheight="66" tilecount="5" columns="5">
 <image source="Knight.png" width="290" height="66"/>
 <tile id="0">
  <properties>
   <property name="idle" value=""/>
  </properties>
  <animation>
   <frame tileid="0" duration="150"/>
   <frame tileid="1" duration="150"/>
   <frame tileid="2" duration="150"/>
   <frame tileid="1" duration="150"/>
  </animation>
 </tile>
 <tile id="3">
  <properties>
   <property name="walk" value=""/>
  </properties>
  <animation>
   <frame tileid="0" duration="150"/>
   <frame tileid="3" duration="150"/>
   <frame tileid="4" duration="150"/>
   <frame tileid="3" duration="200"/>
  </animation>
 </tile>
</tileset>
