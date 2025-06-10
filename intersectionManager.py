# کلاس وسیله نقلیه
class Vehicle:
    def __init__(self, vehicle_type, priority_level):
        self.vehicle_type = vehicle_type
        self.priority_level = priority_level
        self.wait_time = 0

# کلاس مدیریت تقاطع
class IntersectionManager:
    def __init__(self, lanes):
        self.lanes = lanes  # list of semaphores for each lane
        self.vehicle_queue = []
        self.priority_semaphore = threading.Semaphore(1)

    def request_lane(self, vehicle):
        self.priority_semaphore.acquire()
        # logic for handling vehicle types and priorities
        if vehicle.priority_level == 'ambulance':
            print("Ambulance has full priority.")
        elif vehicle.priority_level == 'bus':
            print("Bus will be given 2 lanes if available.")
        else:
            print("Car is granted one lane.")
        self.priority_semaphore.release()

# تست
def test_intersection_manager():
    intersection_manager = IntersectionManager([1, 1, 1, 1])
    
    # ایجاد وسایل نقلیه با اولویت‌های مختلف
    ambulance = Vehicle('ambulance', 'high')
    bus = Vehicle('bus', 'medium')
    car = Vehicle('car', 'low')
    
    # درخواست‌ها
    intersection_manager.request_lane(ambulance)
    intersection_manager.request_lane(bus)
    intersection_manager.request_lane(car)
    
test_intersection_manager()
