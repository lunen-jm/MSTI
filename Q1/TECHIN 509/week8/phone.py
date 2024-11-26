  # developed from class example

class Message:
    def __init__(self, sender, recipient, content):
        self.sender = sender
        self.recipient = recipient
        self.content = content

class message_inbox:
    def __init__(self):
        self.messages = []

    def send_message(self, sender, recipient, content):
        message = Message(sender, recipient, content)
        recipient.receive_message(message)
        self.messages.append(message)
        print(f"Message sent from {sender.name} to {recipient.name}: {content}")

class iPhone:
  
  def __init__(self, name, version, phone_number, color, model): # constructor function
    self.name = name
    self.ios_version = version 
    self.phone_number = phone_number
    self.color = color
    self.model = model
    self.files = []
    self.received_messages = []

  def call(self, number):
    print("Calling %s" % number)
    pass

  def airdrop(self, filename, recipient):
    print("Dropping %s" % filename)
    pass 

  def air_receive(self, recipient):
    pass

  def set_name(self, new_name):
    if len(new_name) < 3:
      print("Name must be at least 3 characters long")
      return
    self.name = new_name

  def receive_message(self, message):
    self.received_messages.append(message)
    print(f"Message received by {self.name} from {message.sender.name}: {message.content}")

  def print_received_messages(self):
    for message in self.received_messages:
        print(f"From {message.sender.name}: {message.content}")



ians_iphone = iPhone(
  name = "Ian's iPhone",
  version = "18",
  phone_number = "123-456-7890",
  color = "black",
  model = "iPhone 16 Pro Max"
  )

rishis_iphone = iPhone(
  name="Rishi's iPhone",
  version = "18", 
  phone_number = "987-654-3210",
  color = "red",
  model = "iPhone 16 Pro Max"
  )

print(ians_iphone.name)

ians_iphone.name = "Ian's iPhone 2" # not a good strategy
ians_iphone.set_name("Ian's iPhone 3") # made a method to use instead
print(ians_iphone.name)

ians_iphone.airdrop("notes.pdf", rishis_iphone)

inbox = message_inbox()

inbox.send_message(ians_iphone, rishis_iphone, "Hello Rishi")
rishis_iphone.print_received_messages()

