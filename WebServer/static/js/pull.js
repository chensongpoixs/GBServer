'use strict';




var remoteVideo = document.getElementById("remoteVideo");
var pullBtn = document.getElementById("StartpullBtn");
var stopPullBtn = document.getElementById("stopPullBtn");

var pullCaptureBtn = document.getElementById("pullCaptureBtn");



var pullSendDataBtn = document.getElementById("PullSendDataBtn");

 
pullSendDataBtn.addEventListener("click", PullSendDataChannel);

pullBtn.addEventListener("click", pullStream);
stopPullBtn.addEventListener("click", stopPull);

pullCaptureBtn.addEventListener("click",  CaptureBtnpull);
 
var audio = $("#audioCheckbox").val();
var video = $("#video").val();

var offer = "";
var pc;
const config = {};
var remoteStream;
var captureType = 1;

var dcClient;

let audioElem ;
//本地视频流
//var localStream = null;
var lastConnectionState = "";

 


/**
 * 从URL查询参数中获取指定参数的值
 * 
 * @param {string} name - 要获取的参数名称
 * @returns {string|null} 参数的值，如果参数不存在则返回null
 */
function getUrlParam(name) {
    const reg = new RegExp(`(^|&)${name}=([^&]*)(&|$)`);
    const r = window.location.search.substr(1).match(reg);
    if (r != null) return decodeURIComponent(r[2]);
    return null;
}

function PullSendDataChannel()
{
	var sendData = $("#pullDataChannel").val();
	console.log('pull send data channel readyState = ', dcClient.readyState);
	  if( dcClient &&  dcClient.readyState == 'open'){
            //console.log('Sending data on dataconnection', self.dcClient)
             dcClient.send(sendData);
        }
}


function CaptureBtnpull()
{
	if (captureType == 1)
	{
		captureType = 0;
		
	}
	else 
	{
		captureType = 1;
	}
}

function stopPull() {
     

}

function sendOffer(offerSdp) {
	//var rtc_api_server = $("#PullhttpUrl").val();
	//var StreamUrl = $("#PullStreamUrl").val();
	 const rtc_ip = getUrlParam('RtcIp');
    const rtc_port = getUrlParam('RtcPort');


    const app_name = getUrlParam('app'); // 'John'
    const stream_name = getUrlParam('StreamName'); // 'John'

	const rtc_api_server = 'https://' + rtc_ip + ':' + rtc_port;
	var StreamUrl = 'webrtc://'+rtc_ip +':'+rtc_port+'/' + app_name + '/' + stream_name;
	console.log("captureType:"+captureType);
    console.log("send offer: /RtcApi/send  offer "+ rtc_api_server);

	// 创建一个新的XMLHttpRequest对象
	var xhr = new XMLHttpRequest();
	// 打开一个新的请求
	xhr.open('POST', rtc_api_server+'/api/rtc/play', true);
	// 设置请求头，指定发送的数据类型
	xhr.setRequestHeader('Content-Type', 'application/json');
	// 创建要发送的数据对象
	var data = {
		type: 'offer',
		sdp: offerSdp,
		streamurl: StreamUrl,
		caputretype: captureType
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


function pullsetupDataChannelCallbacks(datachannel)
{

	 try {
            // Inform browser we would like binary data as an ArrayBuffer (FF chooses Blob by default!)
            datachannel.binaryType = "arraybuffer";

            datachannel.addEventListener('open', e => {
                console.log(`pull Data channel connected: ${datachannel.label}(${datachannel.id}) OK !!!`);
               // if(self.onDataChannelConnected){
                 //   self.onDataChannelConnected();
                //}

				//console.log('')
            });

            datachannel.addEventListener('close', e => {
                console.log(`pull Data channel disconnected: ${datachannel.label}(${datachannel.id}`, e);
            });

            datachannel.addEventListener('message', e => {
				console.log('pull onDataChannelMessage: ', e.data);
               // if (self.onDataChannelMessage){
                //    self.onDataChannelMessage(e.data);
                //}
            });

            datachannel.addEventListener('error', e => {
                console.error(`pull Data channel error: ${datachannel.label}(${datachannel.id}`, e);
            });

            return datachannel;
        } catch (e) { 
            console.warn('pull Datachannel setup caused an exception: ', e);
            return null;
        }
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

        $("#pullTips2").html("Connection Status: " + state);
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
       // remoteStream = e.stream;
       // remoteVideo.srcObject = e.stream;
	   if (e.track)
		{
			console.log('Got track - ' + e.track.kind + ' id=' + e.track.id + ' readyState=' + e.track.readyState); 
		}
		//存放远端视频流
		if (e.track.kind == 'audio')
		{
			handleOnAudioTrack(e.streams[0]);
		}
		 if (e.track.kind == 'video'&& remoteVideo.srcObject !== e.streams[0])
		{
			remoteStream = e.streams;
		
			// 远端视频标签与远端视频流绑定
			remoteVideo.srcObject = e.streams[0];
		}	
	
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

   // create data channel 
		let datachannel = pc.createDataChannel('chat', {ordered: true});
		console.log('pull Created datachannel chat data channel OK !!! ');
		dcClient = pullsetupDataChannelCallbacks(datachannel);
   CreateOfferDescriptionSuccess();
}


function handleOnAudioTrack  ( audioMediaStream)
{
	//console.log('handleOnAudioTrack===>>>>');
	// do nothing the video has the same media stream as the audio track we have here (they are linked)
	if(remoteVideo.srcObject == audioMediaStream)
	{
		console.log('Created new audio element  failed !!!  .');
		//return;
	}
	// video element has some other media stream that is not associated with this audio track
	else if( remoteVideo.srcObject !== audioMediaStream)
	{
		// create a new audio element
		audioElem = document.createElement("Audio");
		audioElem.srcObject = audioMediaStream;

		// there is no way to autoplay audio (even muted), so we defer audio until first click
		//if(!self.autoPlayAudio) {
		//
		 //let clickToPlayAudio = function() {
		 //	audioElem.play();
		 //	remoteVideo.removeEventListener("click", clickToPlayAudio);
		 //};
		 //
		 //remoteVideo.addEventListener("click", clickToPlayAudio);
		//}
		//// we assume the user has clicked somewhere on the page and autoplaying audio will work
		//else 
		//{
			audioElem.play();
		//}
		console.log('Created new audio element to play seperate audio stream.');
	}

}


function CreateOfferDescriptionSuccess() {
    console.log("pc  create  sdp success");
    pc.createOffer().then(
        createSessionDescriptionSuccess,
        createSessionDescriptionError               
    );
}

function createSessionDescriptionSuccess(offer) {
    console.log("offer sdp: \n" , offer.sdp);
	//console.log(offer.sdp);
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


