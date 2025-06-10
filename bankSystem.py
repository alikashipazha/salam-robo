import threading
import time
from collections import deque

# کلاس‌های مشتری و صف انتظار
class Customer:
    def __init__(self, id, arrival_time):
        self.id = id
        self.arrival_time = arrival_time
        self.ticket_number = None  # شماره نوبت

class WaitingQueue:
    def __init__(self):
        self.queue = deque()
        self.queue_semaphore = threading.Semaphore(1)  # دسترسی انحصاری به صف
        self.ticket_semaphore = threading.Semaphore(1)  # دسترسی انحصاری به شماره نوبت
        self.current_ticket_number = 1  # شماره نوبت فعلی

    def add_customer(self, customer):
        self.ticket_semaphore.acquire()  # دسترسی انحصاری به شماره نوبت
        customer.ticket_number = self.current_ticket_number
        self.current_ticket_number += 1
        self.ticket_semaphore.release()
        
        # اضافه کردن مشتری به صف
        self.queue_semaphore.acquire()
        self.queue.append(customer)
        self.queue_semaphore.release()

    def serve_customer(self):
        self.queue_semaphore.acquire()
        if self.queue:
            customer = self.queue.popleft()
            self.queue_semaphore.release()
            return customer
        self.queue_semaphore.release()
        return None

# کلاس کارمند باجه
class Clerk:
    def __init__(self, waiting_queue, id):
        self.waiting_queue = waiting_queue
        self.id = id
        
    def serve_next_customer(self):
        while True:
            customer = self.waiting_queue.serve_customer()
            if customer:
                print(f"Clerk {self.id} is serving customer {customer.id} with ticket number {customer.ticket_number}.")
                time.sleep(1)  # شبیه‌سازی زمان خدمت‌دهی
            else:
                print(f"Clerk {self.id} is waiting for the next customer.")
                time.sleep(2)  # شبیه‌سازی زمان انتظار برای مشتری
                continue

# تابع برای شبیه‌سازی ورود مشتری‌ها با فشار دادن دکمه "n"
def customer_arrival(waiting_queue, customer_id):
    arrival_time = time.time()
    customer = Customer(customer_id, arrival_time)
    waiting_queue.add_customer(customer)
    print(f"Customer {customer_id} arrived and received ticket number {customer.ticket_number}.")
    time.sleep(1)  # شبیه‌سازی زمان بین ورود مشتری‌ها

# تابع برای گرفتن ورودی از کاربر
def listen_for_customers(waiting_queue):
    customer_id = 1
    while True:
        user_input = input("Press 'n' to add a new customer or 'q' to quit:\n")  # ورودی از کاربر
        if user_input.lower() == 'n':
            print("New customer arriving...")
            customer_arrival(waiting_queue, customer_id)
            customer_id += 1
        elif user_input.lower() == 'q':
            print("Exiting the system.")
            break
        time.sleep(0.5)  # جلوگیری از وارد شدن مشتری‌های متعدد به طور همزمان

# تابع تست
def test_system():
    # ایجاد صف انتظار
    waiting_queue = WaitingQueue()
    
    # ایجاد کارمندان باجه
    clerk1 = Clerk(waiting_queue, 1)
    clerk2 = Clerk(waiting_queue, 2)

    # ایجاد ترد برای کارمندان
    clerk_thread1 = threading.Thread(target=clerk1.serve_next_customer)
    clerk_thread2 = threading.Thread(target=clerk2.serve_next_customer)
    clerk_thread1.start()
    clerk_thread2.start()

    # ایجاد ترد برای گرفتن ورودی از کاربر
    customer_thread = threading.Thread(target=listen_for_customers, args=(waiting_queue,))
    customer_thread.start()

    # منتظر ماندن برای پایان تردها
    clerk_thread1.join()
    clerk_thread2.join()
    customer_thread.join()

# شروع برنامه
test_system()
