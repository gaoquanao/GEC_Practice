var globalApp = getApp()
//发送数据到Linux
function sendDataToLinux(buffer){
     //向socket发送buffer数据(一次性8个字节)
     sendSocketMessage(buffer);
	 
	function sendSocketMessage(msg) 
	{
      if (globalApp.globalData.socketOpen) 
      {
		console.log('WebSocket打开成功！');
		
        wx.sendSocketMessage({
			data: msg
        })
      }else{
        console.log('WebSocket打开失败！');
      }
    }
}
//导出模块 便于其他模块require
module.exports = {
  sendDataToLinux : sendDataToLinux
}