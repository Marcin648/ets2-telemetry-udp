
import EtsTelemetryClient

if __name__ == "__main__":
    telemetry = EtsTelemetryClient.Client()
    telemetry.connect("127.0.0.1", 23444)
    print("")
    while(True):
        telemetry.update()
        print("Speed: %f" % (telemetry.truck.speed * 3.6))
        print("Trailer speed: %f" % (telemetry.trailer.wheel_velocity[0] * 3.6))
    telemetry.close()
    pass
