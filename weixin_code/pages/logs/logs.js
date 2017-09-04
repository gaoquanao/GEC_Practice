//logs.js  不能省略js后缀
var util = require('../../utils/util.js');
Page({
  data: {
    logs: []
  },
  // 页面初始化的时候，先加载日志记录，然后存储到logs数组中
  onLoad: function () {
    this.setData({
      logs: (wx.getStorageSync('logs') || []).map(function (log) {
        return util.formatTime(new Date(log));
      })
    })
  }
})
