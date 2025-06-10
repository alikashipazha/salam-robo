# کلاس درخواست
class ReservationRequest:
    def __init__(self, request_type, num_rooms):
        self.request_type = request_type  # 'normal' or 'VIP'
        self.num_rooms = num_rooms

# کلاس رزرو
class ConferenceRoomManager:
    def __init__(self, num_rooms, room_capacity):
        self.num_rooms = num_rooms
        self.room_capacity = room_capacity
        self.rooms = [0] * num_rooms  # 0 means available
        self.reservation_semaphore = threading.Semaphore(1)

    def reserve_rooms(self, request):
        self.reservation_semaphore.acquire()
        if request.request_type == 'VIP':
            if self.num_rooms >= request.num_rooms:
                print(f"VIP request: Rooms reserved for {request.num_rooms} rooms.")
            else:
                print("Not enough rooms for VIP request.")
        elif request.request_type == 'normal':
            if self.num_rooms >= 1:
                print(f"Normal request: One room reserved.")
            else:
                print("No rooms available for normal request.")
        self.reservation_semaphore.release()

# تست
def test_conference_room_manager():
    conference_manager = ConferenceRoomManager(3, 50)
    
    # درخواست‌های مختلف
    vip_request = ReservationRequest('VIP', 2)
    normal_request = ReservationRequest('normal', 1)
    
    # رزرو اتاق‌ها
    conference_manager.reserve_rooms(vip_request)
    conference_manager.reserve_rooms(normal_request)
    
test_conference_room_manager()
