import React, {useState} from 'react';
import {inject, observer} from "mobx-react";
import {KeyboardVoice} from "@mui/icons-material";
import {IconButton} from "@mui/material";
import {Microphone} from "microphone-js";

const mic = new Microphone();

function AudioRecorder({disabled, onFinish}) {
    const [enable, setEnable] = useState(false);

    function handleStart() {
        mic.reset();
        mic.start();
    }

    function handleFinish() {
        mic.stop();
        const blob = mic.getBlob();
        onFinish(blob);
    }

    const handleMic = () => {
        if (enable) handleFinish();
        else handleStart();
        setEnable(e => !e);
    }

    return <React.Fragment>
        <IconButton size={"small"} flex={0}
                    disabled={disabled}
                    color={enable ? 'success' : 'default'}
                    onClick={handleMic}>
            <KeyboardVoice/>
        </IconButton>
    </React.Fragment>
}

export default inject()(observer(AudioRecorder));
