import RPi.GPIO as GPIO
import os
import picamera
import pygame
import time

LED_PIN = 12

class pyscope :
    screen = None

    def __init__(self):
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(LED_PIN, GPIO.OUT)
        self.led_state = False
        "Ininitializes a new pygame screen using the framebuffer"
        disp_no = os.getenv('DISPLAY')
        if disp_no:
            print("I'm running under X display = {0}".format(disp_no))

        # Check which frame buffer drivers are available
        drivers = ["X11"] #TODO
        found = False
        for driver in drivers:
            if not os.getenv('SDL_VIDEODRIVER'):
                os.putenv('SDL_VIDEODRIVER', driver)
            try:
                pygame.display.init()
            except pygame.error:
                print('Driver: {0} failed.'.format(driver))
                continue
            found = True
            break
        if not found:
            raise Exception('No suitable video driver found!')

        # TODO Initialize mixer
        pygame.mixer.init()
        time.sleep(1)
        pygame.mixer.music.load("./lab8.mp3")

		# change resolution here
        size = (800,600)
        print("Framebuffer size: %d x %d" % (size[0], size[1]))
        self.screen = pygame.display.set_mode(size)
        # Clear the screen to start
        self.screen.fill((0, 0, 0))
        # Initialize font support
        pygame.font.init()
        # Update the screen
        pygame.display.update()


    def __del__(self):
        "Destructor to make sure pygame shuts down, etc."
        GPIO.cleanup()

    def capture_img(self):
        with picamera.PiCamera() as cam:
            cam.resolution = (800,600)
            cam.capture('image.jpg', resize = (800, 600))

    def create_button(self):
        led_button = pygame.Rect(10, 10, 40, 40)
        music_button = pygame.Rect(80, 10, 40, 40)
        pygame.draw.rect(self.screen, (0, 0, 100), led_button)
        pygame.draw.rect(self.screen, (20, 0, 0), music_button)

    def display_img(self):
        black = (0,0,0)
        white = (255,255,255)
        red = (255,0,0)
        green = (0,255,0)
        blue = (0,0,255)
        self.screen.fill(white)
        img = pygame.image.load('image.jpg')
        self.screen.blit(img,(0,0))
		#TODO: create buttons
        self.create_button()

		#TODO: handle events
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pass
            elif event.type == pygame.MOUSEBUTTONDOWN:
                pos = pygame.mouse.get_pos()
                if pos[0] >= 10 and pos[0] <= 50 and pos[1] >= 10 and pos[1] <= 50:
                    self.led_state  = not self.led_state
                    if self.led_state == True:
                        GPIO.output(LED_PIN, GPIO.HIGH)
                    else:
                        GPIO.output(LED_PIN, GPIO.LOW)
                if pos[0] >= 80 and pos[0] <= 120 and pos[1] >= 10 and pos[1] <= 50:
                    print("click music")
                    pygame.mixer.music.play()

        pygame.display.flip()
        pygame.display.update()
# Create an instance of the PyScope class
scope = pyscope()
while True:
    scope.capture_img()
    scope.display_img()
