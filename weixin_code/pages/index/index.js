var sendData = require('../../utils/sendData.js');
Page({
  data: {
     isOpenLed : false,//LED灯控制
     isOpenWindow : false, //窗控制
     isOpenUi : false, //UI界面控制
     toArm : 1, //发送给Arm
     src: "../../image/light_close.png"
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    //options表示传递过来的参数对象
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {
  
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
  
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {
  
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
  
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {
  
  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {
  
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
  
  },

  ledCtrollSwitch : function (){
    var that = this;

    var temp =  "../../image/light_close.png";

    if (this.data.isOpenLed)
    {
      temp = "../../image/light_close.png";
    }
    else
    {
      temp = "../../image/light_open.png";
    }

    this.setData({
      isOpenLed: !that.data.isOpenLed,
      // src: this.data.isOpenLed ? "../../image/light_open.png" : "../../image/light_close.png"
      src: temp
      
    })

	  //定义发送的数据(申请8个字节的内存空间，即64个二进制位，此时buffer是空的)
	  var buffer = new ArrayBuffer(8);

	  var socketMsgQueue = new Uint32Array(buffer);

	  socketMsgQueue[1] = that.data.isOpenLed ? 0x1 : 0x0;// value占后32个字节
      
    var socketMsg_u8 =  new Uint8Array(buffer); 
    socketMsg_u8[0] = 0x0; //保留位(未使用)
    socketMsg_u8[1] = 0x0; //UI界面
    socketMsg_u8[2] = 0x0; //功能位CMD(可能是LED、窗、界面控制)
    socketMsg_u8[3] = that.data.toArm; //发给谁Dst(可能发送给单板或者小程序)
    console.log("send ", that.data.isOpenLed);
    sendData.sendDataToLinux(buffer);

    socketMsgQueue = [];
},
  //窗控制开关
  windowCtrollSwitch : function (){

    var that = this;
    that.setData({
      isOpenWindow: !that.data.isOpenWindow
    })

      var buffer = new ArrayBuffer(8);

      var socketMsgQueue = new Uint32Array(buffer);

      socketMsgQueue[1] = that.data.isOpenWindow ? 0x1 : 0x0;// value占后32个字节

      var socketMsg_u8 = new Uint8Array(buffer);
      socketMsg_u8[0] = 0x0; //保留位(未使用)
      socketMsg_u8[1] = 0x0; //UI界面
      socketMsg_u8[2] = 0x1; //功能位CMD(可能是LED、窗、界面控制)
      socketMsg_u8[3] = that.data.toArm; //发给谁Dst(可能发送给单板或者小程序)

	  //发送数据到Linux
      sendData.sendDataToLinux(buffer);
      socketMsgQueue = []
  },

  uiCtrollSwitch : function (){
    var that = this;

    that.setData({
      isOpenUi: !that.data.isOpenUi
    })

    var buffer = new ArrayBuffer(8);

    var socketMsgQueue = new Uint32Array(buffer);

    socketMsgQueue[1] = that.data.isOpenUi ? 0x1 : 0x0;// value占后32个字节

    var socketMsg_u8 = new Uint8Array(buffer);
    socketMsg_u8[0] = 0x0; //保留位(未使用)
    socketMsg_u8[1] = 0x0; //UI界面
    socketMsg_u8[2] = 0x2; //功能位CMD(可能是LED、窗、界面控制)
    socketMsg_u8[3] = that.data.toArm; //发给谁Dst(可能发送给单板或者小程序)

    sendData.sendDataToLinux(buffer);

    socketMsgQueue = []
  }
})