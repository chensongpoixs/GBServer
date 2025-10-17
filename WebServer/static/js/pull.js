'use strict';




var remoteVideo = document.getElementById("remoteVideo");
var pullBtn = document.getElementById("StartpullBtn");
var stopPullBtn = document.getElementById("stopPullBtn");

pullBtn.addEventListener("click", startPull);
stopPullBtn.addEventListener("click", stopPull);

var StreamUrl = $("#StreamUrl").val();
var clientId = $("#clientId").val();
var audio = $("#audioCheckbox").val();
var video = 1;//$("#video").val();
var offer = "";
var pc;
const config = {};
var remoteStream;
//本地视频流
var localStream = null;
var lastConnectionState = "";

function startPull() {
    console.log("send pull: /RtcApi/pull");

pullStream();
    
}

function stopPull() {
     

}

function sendOffer(offerSdp) {
	var rtc_api_server = $("#PullhttpUrl").val();
    console.log("send offer: /RtcApi/send  offer "+ rtc_api_server);

	// 创建一个新的XMLHttpRequest对象
var xhr = new XMLHttpRequest();
// 打开一个新的请求
xhr.open('POST', rtc_api_server+'/rtc/play/', true);
// 设置请求头，指定发送的数据类型
xhr.setRequestHeader('Content-Type', 'application/json');
// 创建要发送的数据对象
var data = {
   type: 'offer',
   sdp: offerSdp,
   streamurl: StreamUrl,
   clientid: clientId
};
console.log('JSON.stringify(data) :' + JSON.stringify(data));
// 发送请求并将数据转换为JSON字符串
xhr.send(JSON.stringify(data));
// 设置请求完成时的回调函数
xhr.onreadystatechange = function() {
   if (xhr.readyState === 4 && xhr.status === 200) {
       // 请求成功，处理响应数据
       console.log(xhr.responseText);
	   var ret_data =  JSON.parse(xhr.responseText)
	   console.log('ret_data :' + ret_data.sdp);
	   pc.setRemoteDescription(ret_data).then(
        setRemoteDescriptionSuccess,
        setRemoteDescriptionError
    );
   }
};
	
	
}

function pullStream() {
    pc = new RTCPeerConnection(config);
    pc.oniceconnectionstatechange = function(e) {
        var state = "";
        if (lastConnectionState != "") {
            state = lastConnectionState + "->" + pc.iceConnectionState;
        } else {
            state = pc.iceConnectionState;
        }

        $("#tips2").html("连接状态: " + state);
        lastConnectionState = pc.iceConnectionState;
    }
     pc.onsignalingstatechange = function(state)
		{
			 //console.log('iceconnectionstatechange ---> [' +  self.pcClient.iceConnectionState + '] ^_^ !!!');
            console.info('signaling state change:', state)
        } 

         

        pc.onicegatheringstatechange = function(state) {
			// console.log('iceconnectionstatechange ---> [' +  self.pcClient.iceConnectionState + '] ^_^ !!!');
            console.info('ice gathering state change:', state)
        } 
		  pc.addTransceiver("audio", { direction: "recvonly" });
		    pc.addTransceiver("video", { direction: "recvonly" });
    pc.ontrack = function(e) {
        remoteStream = e.stream;
        remoteVideo.srcObject = e.stream;
		//存放远端视频流
	//remoteStream = e.streams[0];
	
	// 远端视频标签与远端视频流绑定
	//remoteVideo.srcObject = e.streams[0];
    }
    //将本地音视频流中所有tranck添加到PeerConnection对象中
	//localStream.getTracks().forEach((track) => {
	//	pc.addTrack(track, localStream);
	//});
    console.log("creat offer  sdp start");

   // pc.setRemoteDescription(offer).then(
   //     setRemoteDescriptionSuccess,
   //     setRemoteDescriptionError
   // );
   CreateOfferDescriptionSuccess();
}

function CreateOfferDescriptionSuccess() {
    console.log("pc  create  sdp success");
    pc.createOffer().then(
        createSessionDescriptionSuccess,
        createSessionDescriptionError               
    );
}

function createSessionDescriptionSuccess(offer) {
    console.log("offer sdp: \n" + offer);
	console.log(offer);
    console.log("pc set local sdp");
    pc.setLocalDescription(offer).then(
        setLocalDescriptionSuccess,
        setLocalDescriptionError
    );

    sendOffer(offer.sdp);
}

function setLocalDescriptionSuccess() {
    console.log("set local sdp success");
}


function setRemoteDescriptionSuccess()
{
	  console.log("set answer sdp success");
}
function setRemoteDescriptionError(error) {
    console.log("pc set remote sdp error: " + error);
}

function setLocalDescriptionError(error) {
    console.log("pc set local sdp error: " + error);
}

function createSessionDescriptionError(error) {
    console.log("pc create offer error: " + error);
}


