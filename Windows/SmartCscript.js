function convertByteArrayToString(bytearray) {
var res = "";
  if (!bytearray)
    return;
  for (var i=0; i<bytearray.length; i++)
    res += ((bytearray[i]<16) ? "0" : "") + bytearray[i].toString(16);

  return res;
}

function convertStringToByteArray(str) {
  var byteArray = [];
  if (str.length % 2)             
    return;
  if (str.indexOf("0x") == 0 || str.indexOf("0X") == 0)
    str = str.substring(2);
  for (var i = 0; i<str.length; i += 2)
    byteArray[Math.floor(i/2)] = parseInt(str.slice(i, i+2), 16);
  return byteArray;
}

function SmartC(form1) {
	try {
		var stringAPDU = document.form1.input.value;
		// normally Firefox extensions implicitly have XPCOM privileges, but since this is a file we have to request it.
		netscape.security.PrivilegeManager.enablePrivilege("UniversalXPConnect");
		const cid = "@SmartCAuth.com/SmartCPlugin/SmartC;1";
		obj = Components.classes[cid].createInstance();
		// var xpcom = Components.classes[cid].getService(Components.interfaces.ISmartC);
		// var array = xpcom.SendAPDU(apdu.length,apdu,{});
		// bind the instance we just created to our interface
		obj = obj.QueryInterface(Components.interfaces.ISmartC);
		var apdu =[];
		apdu = convertStringToByteArray(stringAPDU);
		// apdu[0] = 0x00; apdu[1] = 0xa4; apdu[2] = 0x04; apdu[3] = 0x00; apdu[4] = 0x0a; apdu[5] = 0xa0; apdu[6] = 0x00; apdu[7] = 0x00;
		// apdu[8] = 0x00;  apdu[9] = 0x62; apdu[10] = 0x03; apdu[11] = 0x01; apdu[12] = 0x0c; apdu[13] = 0x06; apdu[14] = 0x01;
		// 00A404000AA00000006203010C0601
		// 00b0000080
		 
	} catch (err) {
		alert(err);
		return; 
	}
		var res = obj.SendAPDU(apdu.length,apdu,{});
		var byteAPDU = convertByteArrayToString(res);
		alert("You have sent the APDU : "+ stringAPDU);
		alert("Smart card answer : "+ byteAPDU);
			
}
