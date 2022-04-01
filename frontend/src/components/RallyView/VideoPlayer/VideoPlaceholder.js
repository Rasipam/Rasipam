import React from 'react';
import {inject, observer} from "mobx-react";
import {Box, Typography} from "@mui/material";
import {BrokenImage, Pending, PlayCircle} from "@mui/icons-material";
import {useTranslation} from "react-i18next";
import strings from "../../../static/strings";

export const VideoStatus = {
    Pending: 'Pending',
    Error: 'Error',
    Ready: 'Ready',
    Loading: 'Loading',
}

const hints = {
    [VideoStatus.Pending]: {icon: PlayCircle, text: strings.videoHintPending},
    [VideoStatus.Error]: {icon: BrokenImage, text: strings.videoHintError},
    [VideoStatus.Loading]: {icon: Pending, text: strings.videoHintLoading},
}

function VideoPlaceholder({status}) {
    const {t} = useTranslation();
    const hint = hints[status];
    return <Box position={'relative'}
                pt={`${9 / 16 * 100}%`}
                height={0}>
        <Box position={'absolute'}
             top={0} left={0} right={0} bottom={0}
             display={'flex'} flexDirection={'column'}
             justifyContent={'center'} alignItems={'center'}
             sx={{backgroundImage: 'linear-gradient(#131313, #353535, #131313)'}}>
            <hint.icon sx={{
                m: 3,
                color: 'white',
                fontSize: '4rem',
            }}/>
            <Typography color={'white'}>
                {t(hint.text)}
            </Typography>
        </Box>
    </Box>
}

export default inject()(observer(VideoPlaceholder));
