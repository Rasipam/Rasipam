import React, {useCallback, useEffect, useRef, useState} from 'react';
import VideoPlaceholder, {VideoStatus} from "./VideoPlaceholder";
import {Box} from "@mui/material";

function VideoPlayer({src, startTime, endTime}) {
    const [status, setStatus] = useState(VideoStatus.Pending);
    useEffect(() => {
        if (src) setStatus(VideoStatus.Loading);
        else setStatus(VideoStatus.Pending);
    }, [src]);
    const handleError = () => setStatus(VideoStatus.Error);
    const handleLoaded = () => setStatus(VideoStatus.Ready);

    const videoRef = useRef(null);
    useEffect(() => {
        if (status !== VideoStatus.Ready) return;
        if (!videoRef.current) return;
        videoRef.current.currentTime = startTime;
    }, [status, startTime])
    const handleTimeUpdate = useCallback(() => {
        if (videoRef.current.currentTime > endTime)
            videoRef.current.pause();
    }, [endTime])

    return <Box width={'100%'}>
        {[VideoStatus.Error, VideoStatus.Pending, VideoStatus.Loading].includes(status) &&
            <VideoPlaceholder status={status}/>}
        {[VideoStatus.Loading, VideoStatus.Ready].includes(status) &&
            <video ref={videoRef}
                   width={'100%'}
                   controls
                   style={{display: status === VideoStatus.Loading ? 'none' : 'block'}}
                   onLoadedMetadata={handleLoaded}
                   onTimeUpdate={handleTimeUpdate}
                   onError={handleError}>
                <source src={src}/>
            </video>}
    </Box>
}

export default VideoPlayer;
