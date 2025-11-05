// 'use strict';

var localVideo = document.getElementById("localVideo");
var pushBtn = document.getElementById("startPushBtn");
var stopPushBtn = document.getElementById("stopPushBtn");

pushBtn.addEventListener("click", startPush);
stopPushBtn.addEventListener("click", stopPush);

if (adapter.browserDetails.browser === 'chrome' &&
    adapter.browserDetails.version >= 107) {
  // See https://developer.chrome.com/docs/web-platform/screen-sharing-controls/
  //document.getElementById('options').style.display = 'block';
} else if (adapter.browserDetails.browser === 'firefox') {
  // Polyfill in Firefox.
  // See https://blog.mozilla.org/webrtc/getdisplaymedia-now-available-in-adapter-js/
  adapter.browserShim.shimGetDisplayMedia(window, 'screen');
}






var clientId = $("#clientId").val();
var pushStreamUrl = $("#PushStreamUrl").val();
var audio = $("#audio").val();
var video = $("#video").val();
var offer = "";
var push_pc;
//const config = {};
var localStream;
var lastConnectionState = "";

function startPush() {
    console.log("=========open_device=============");
    open_device();
    
}




/**
 功能： 打开有音视频设备，
 
*/ 
function open_device()
{
    //sendPushOffer('');
	if (!navigator.mediaDevices || !navigator.mediaDevices.getUserMedia)
	{
		console.log('the getUserMedia is not supported !!!');
		return;
	}
	
	var constraints;
	constraints = {
		video : true,
       // audio: true
		 audio :{
		 	echoCancellation : true,
		 	noiseSuppression: true,
		 	autoGainControl: true
		 }
	};

	
	
	// 底层的api 设置回调函数 getMediaStream
	navigator.mediaDevices.getUserMedia(constraints)
					.then(getMediaStream)
					.catch(handleError);
	


    
}


/**
 功能：错误处理函数
 返回值: 无
*/

function handleError(err)
{
	console.log('Failed to get Media Stream !', err);
}



/**
 功能： 打开音视频设备成功时的回调用
 返回值: true 

*/
function getMediaStream(stream)
{
	//将从设备上获取到的音视频track添加到localStream中
	if (localStream)
	{
		stream.getAudioTracks().forEach((track) => {
			localStream.addTrack(track);
			stream.removeTrack(track);
			
		});
	}
	else 
	{
		localStream = stream;
	}
	
	//本地视频标签与本地流绑定
	localVideo.srcObject = localStream;
	
	/**
	 调用 conn() 函数的位置特别重要，一定要在
	 getMediaStream调用之后再调用它，否则就
	 会出现绑定失败的情况
	*/
	console.log('=====================getMediaStream===============');
	// setup connection
	//conn();
    pushStream();
}

function stopPush() {
    console.log("send stop push: /signaling/stoppush");

    localVideo.srcObject = null;
    if (localStream && localStream.getAudioTracks()) {
        localStream.getAudioTracks()[0].stop();
    }

    if (localStream && localStream.getVideoTracks()) {
        localStream.getVideoTracks()[0].stop();
    }

    if (pc) {
        pc.close();
        pc = null;
    }

    // $("#tips1").html("");
    // $("#tips2").html("");
    // $("#tips3").html("");

    // $.post("/signaling/stoppush",
    //     {"uid": uid, "streamName": streamName},
    //     function(data, textStatus) {
    //         console.log("stop push response: " + JSON.stringify(data));
    //         if ("success" == textStatus && 0 == data.errNo) {
    //             $("#tips1").html("<font color='blue'>停止推流请求成功!</font>");
    //         } else {
    //             $("#tips1").html("<font color='red'>停止推流请求失败!</font>");
    //         }
    //     },
    //     "json"
    // );

}

function sendAnswer(answerSdp) {
    // console.log("send answer: /signaling/sendanswer");

    // $.post("/signaling/sendanswer",
    //     {"uid": uid, "streamName": streamName, "answer": answerSdp, "type": "push"},
    //     function(data, textStatus) {
    //         console.log("send answer response: " + JSON.stringify(data));
    //         if ("success" == textStatus && 0 == data.errNo) {
    //             $("#tips3").html("<font color='blue'>answer发送成功!</font>");
    //         } else {
    //             $("#tips3").html("<font color='red'>answer发送失败!</font>");
    //         }
    //     },
    //     "json"
    // );
}

function pushStream() {
    // config
    push_pc = new RTCPeerConnection(null);
    push_pc.oniceconnectionstatechange = function(e) {
        var state = "";
        if (lastConnectionState != "") {
            state = lastConnectionState + "->" + push_pc.iceConnectionState;
        } else {
            state = push_pc.iceConnectionState;
        }

        $("#tips2").html("连接状态: " + state);
        lastConnectionState = push_pc.iceConnectionState;
    }
     push_pc.onsignalingstatechange = function(state)
		{
			 console.log('iceconnectionstatechange ---> [' +  push_pc.iceConnectionState + '] ^_^ !!!');
            console.info('signaling state change:', state)
        } 

         

    push_pc.onicegatheringstatechange = function(state) {
            console.log('iceconnectionstatechange ---> [' +  push_pc.iceConnectionState + '] ^_^ !!!');
        console.info('ice gathering state change:', state)
    } 

           // pc.addTransceiver("audio", { direction: "sendonly" });
		    //pc.addTransceiver("video", { direction: "sendonly" });
        push_pc.ontrack = function(e) {
       // remoteStream = e.stream;
       // remoteVideo.srcObject = e.stream;
	//    if (e.track)
		{
			console.log('Got track - ' + e.track.kind + ' id=' + e.track.id + ' readyState=' + e.track.readyState); 
		}
		//存放远端视频流
		// if (e.track.kind == 'audio')
		// {
		// 	handleOnAudioTrack(e.streams[0]);
		// }
		//  if (e.track.kind == 'video'&& remoteVideo.srcObject !== e.streams[0])
		// {
		// 	remoteStream = e.streams;
		
		// 	// 远端视频标签与远端视频流绑定
		// 	remoteVideo.srcObject = e.streams[0];
		// }	
	
    }
    // pc.setRemoteDescription(offer).then(
    //     setRemoteDescriptionSuccess,
    //     setRemoteDescriptionError
    // );




   // localVideo
   bindTracks();
   call();
}



/**
	功能： 将音视频track绑定到PeerConnection对象中
*/
function bindTracks()
{
	console.log('bind tracks into RTCPeerConnection !!!');
	
	if (push_pc === null && localStream === undefined)
	{
		console.log('push_pc is null or undefined!!!');
		return;
	}
	
	if (localStream === null && localStream === undefined)
	{
		console.log('localStream is null or undefinded !!!');
		return;
	}
	
	//将本地音视频流中所有tranck添加到PeerConnection对象中
	localStream.getTracks().forEach((track) => {
		push_pc.addTrack(track, localStream);
	});
}




/**
 功能： 开启 "呼叫"
*/

function call()
{
	//if (state === 'joined_conn')
	{
		var offerOptions = {
			offerToReceiveAudio: 1,
			offerToReceiveVideo: 1
		};
		
		/**
		 创建Offer
		 如果成功: 则返回getOffer()方法
		 如果失败：则返回handleOfferError方法
		 
		*/
		push_pc.createOffer(offerOptions)
			.then(getPushOffer)
			.catch(handlePushOfferError);
	}
}
/**
 功能: 处理Offer错误

*/

function handlePushOfferError(err)
{
	console.log('Failed ot carete offer:', err);
}




/**
 功能： 获取Offer SDP 描述符的

*/
function getPushOffer(desc)
{
	console.log('offer desc ==> ', desc );
    console.log(desc.sdp);
	//设置Offer
	push_pc.setLocalDescription(desc);
	


    sendPushOffer(desc.sdp);
	//将Offer显示在网页中出来
	//offer.value = desc.sdp;
	//offerdesc = desc;
	
	//将Offer SDP 发送给对端
	
	// sendMessage(
	// 			{
	// 				msg_id: 208,
	// 				data: offerdesc
	// 			}
	// 		 );
}



function sendPushOffer(offerSdp) {
	var rtc_api_server = $("#PushhttpUrl").val();
	var StreamUrl = $("#PushStreamUrl").val();
	 

	
	console.log("captureType:"+captureType);
    console.log("send offer: /RtcApi/send  offer "+ rtc_api_server);

	// 创建一个新的XMLHttpRequest对象
    var xhr = new XMLHttpRequest();
    // 打开一个新的请求
    xhr.open('POST', rtc_api_server+'/api/rtc/push', true);
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
        push_pc.setRemoteDescription(ret_data).then(
            setPushRemoteDescriptionSuccess,
            setPushRemoteDescriptionError
        );
    }
    };
	
	
}
 
function setPushLocalDescriptionSuccess() {
    console.log("set Push local description success");
}

function setPushRemoteDescriptionSuccess()
{
    console.log('set Push Remote  Descritption success !!!');
}

function setPushRemoteDescriptionError(error) {
    console.log("pc set remote description error: " + error);
}

function setPushLocalDescriptionError(error) {
    console.log("Push pc set local description error: " + error);
}

function createPushSessionDescriptionError(error) {
    console.log("Pushpc create answer error: " + error);
}


