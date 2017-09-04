// pages/temprature/temprature.js
Page({
  data: {
    temperature: 26,
    humidity: 0
  },
  // 页面加载的时候触发
  // 页面初始化 options为页面跳转所带来的参数
  onLoad: function (options) {
    var that = this;
    //定义prot_wx_cmd对象
    var prot_wx_cmd = {
      temperature: 0,
      humidity: 1,
      light: 2,
      MQ_2: 3,
      func_status: 4
    }
    // ====================================
    //监听从服务器端发送过来(到小程序)的消息
    wx.onSocketMessage(function (res) {
      //发送的buffer(即res.data)是8个字节共计64个二进制位
      //dataView_u8是拥有8个数字的数组视图(每个数字是8个bit的无符号数)
      //var dataView_u8 = new Uint8Array(res.data);
      //console.log(dataView_u8);

      //获取到服务器发送过来的8个字节的信息
      var dv = new DataView(res.data);
      //根据约定的协议结构：
      //v0表示保留位(未使用)
      //v1表示UI界面(即温度、湿度、光照、MQ-2、功能状态等信息显示的地方)
      //v2表示功能位(即声明传递过来的信息是哪种信息，可能是温度(0)、湿度(1)、光照(1)、MQ-2(3)或者功能状态信息(4)等5种)
      //v3表示发送给谁(小程序)
      var _dst = dv.getUint8(0, false);
      var _cmd = dv.getUint8(1, false);
      var _ui = dv.getUint8(2, false);
      var _reserve = dv.getUint8(3, false);
      //v4---v7表示value值：可能是温度、湿度、光照、MQ-2或者状态信息的值
      //对于温度、湿度、光照和MQ-2而言，必须一次性读4个字节，单独读一个字节没有意义
      //对于功能状态而言，数据只储存在第4个字节(可以只读第4个字节或者也可以一次性读4个字节，结果一样)
      var _value = dv.getInt32(4, false);

      console.log('目标是：' + _dst);
      console.log('指令是：' + _cmd);
      console.log('UI界面是：' + _ui);
      console.log('保留位是：' + _reserve);
      console.log('信息值是：' + _value);

      //根据约定的协议结构，v1表示UI界面(即明确当前显示的是什么内容)
      //可能是显示温度、湿度、光照、MQ-2、功能状态等
      switch (_cmd) {
        case prot_wx_cmd.temperature:
          console.log('当前温度为' + _value);
          that.setData({
            temperature: _value
          })
          break;
        case prot_wx_cmd.humidity:
          console.log('当前湿度为' + _value);
          that.setData({
            humidity: _value
          })
          break;
        case prot_wx_cmd.light:
          console.log('当前光照为' + _value);
          break;
        case prot_wx_cmd.MQ_2:
          console.log('当MQ_2为' + _value);
          break;
        case prot_wx_cmd.func_status:
          console.log('当前功能状态为' + _value);
          break;
      }
    })
  },
  
  onReady: function () {
    // 页面渲染完成
    //console.log("页面已经准备好了!");
  },
  onShow: function () {
    // 页面显示
  },
  onHide: function () {
    // 页面隐藏
  },
  onUnload: function () {
    // 页面关闭
  }
})
