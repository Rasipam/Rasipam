def video_file(video_name: str):
    with open(f'videos/video_name', 'rb') as f:
        yield from f
