# کلاس دانلود
class Download:
    def __init__(self, user_id):
        self.user_id = user_id
        self.download_speed = 0  # Initially zero

# کلاس مدیریت دانلود
class DownloadManager:
    def __init__(self, max_concurrent_downloads, bandwidth):
        self.max_concurrent_downloads = max_concurrent_downloads
        self.bandwidth = bandwidth
        self.active_downloads = []
        self.waiting_queue = []
        self.download_semaphore = threading.Semaphore(max_concurrent_downloads)

    def request_download(self, user):
        if len(self.active_downloads) < self.max_concurrent_downloads:
            self.start_download(user)
        else:
            self.waiting_queue.append(user)

    def start_download(self, user):
        self.download_semaphore.acquire()
        download = Download(user)
        self.active_downloads.append(download)
        self.allocate_bandwidth()

    def finish_download(self, user):
        self.active_downloads.remove(user)
        self.allocate_bandwidth()

    def allocate_bandwidth(self):
        available_bandwidth = self.bandwidth / len(self.active_downloads)
        for download in self.active_downloads:
            download.download_speed = available_bandwidth

# تست
def test_download_manager():
    download_manager = DownloadManager(3, 100)
    
    # ایجاد کاربران
    user1 = 'user1'
    user2 = 'user2'
    user3 = 'user3'
    user4 = 'user4'
    
    # درخواست دانلود
    download_manager.request_download(user1)
    download_manager.request_download(user2)
    download_manager.request_download(user3)
    download_manager.request_download(user4)
    
    # پایان دانلود‌ها
    download_manager.finish_download(user1)
    download_manager.finish_download(user2)
    
test_download_manager()
