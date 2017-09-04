function formatTime(date) {
  var year = date.getFullYear() //年(四位数)
  var month = date.getMonth() + 1 //月
  var day = date.getDate() //日

  var hour = date.getHours() //时
  var minute = date.getMinutes() //分
  var second = date.getSeconds() //秒
  return [year, month, day].map(formatNumber).join('/') + ' ' 
  + [hour, minute, second].map(formatNumber).join(':');
}

function formatNumber(n) {
  n = n.toString();
  return n[1] ? n : '0' + n;
}

module.exports = {
  formatTime: formatTime
}
