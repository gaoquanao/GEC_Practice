App({
  onLaunch: function (options) {
    var logs = wx.getStorageSync('logs') || []
    logs.unshift(Date.now())
    wx.setStorageSync('logs', logs);
    
    wx.connectSocket({
      // 连接的网络域名
      url: 'wss://gec09.rfish.xyz:12345'
    })

    var that = this;
    //监听打开WebSocket连接打开事件
    wx.onSocketOpen(function (res) {
      that.globalData.socketOpen = true;
      console.log('WebSocket连接已打开！');
    })
  },
  onShow: function (options) {
    // Do something when show.
  },
  onHide: function () {
    // Do something when hide.
  },
  onError: function (msg) {
    console.log(msg)
  },
  globalData: { 
    socketOpen : false
  }
})