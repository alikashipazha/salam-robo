# کلاس فرآیند و تخصیص چاپگر
class Process:
    def __init__(self, priority, id):
        self.priority = priority
        self.id = id

class PrinterManager:
    def __init__(self, num_printers):
        self.num_printers = num_printers
        self.process_queue = []
        self.queue_semaphore = threading.Semaphore(1)

    def request_printer(self, process):
        self.queue_semaphore.acquire()
        self.process_queue.append(process)
        self.process_queue.sort(key=lambda p: p.priority, reverse=True)
        if len(self.process_queue) <= self.num_printers:
            print(f"Process {process.id} is using a printer.")
        else:
            print(f"Process {process.id} is waiting for a printer.")
        self.queue_semaphore.release()

    def release_printer(self):
        self.queue_semaphore.acquire()
        if self.process_queue:
            process = self.process_queue.pop(0)
            print(f"Process {process.id} finished and released the printer.")
        self.queue_semaphore.release()

# تست
def test_printer_manager():
    printer_manager = PrinterManager(2)
    
    # ایجاد فرآیندها با اولویت‌های مختلف
    process1 = Process(1, 101)
    process2 = Process(3, 102)
    process3 = Process(2, 103)
    
    # درخواست چاپگر
    printer_manager.request_printer(process1)
    printer_manager.request_printer(process2)
    printer_manager.request_printer(process3)
    
    # آزادسازی چاپگر
    printer_manager.release_printer()
    printer_manager.release_printer()
    
test_printer_manager()
