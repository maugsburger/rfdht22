# RF Communication Protocol/Procedure

## Terms

* Sensor: autonomous device with probes attached to it, every sensor has its
  own 7-bit-id in range `0x01` to `0xFF`).
    * Probe: Every single measurement device attached to a sensor
* Master: device recieving all measurements for further processing, ID is
  always `0x00`
* PTX: The sender, initiating communication, here every sensors
* PRX: The reciever, can't initiate transfer on it's own, here the master

## Adresses and Data Pipes

The used [nRF24L01+][0] chip has the ability to divide the data stream into up
to six data pipes, identified by unique addresses.

Each address consists of up to 5 bytes, with the following requirements:

<table>
<tr><th></th><th>Byte4</th><th>Byte3</th><th>Byte2</th><th>Byte1</th><th>Byte0</th></tr>
<tr><th>Pipe0</th><td>0b4</td><td>0b3</td><td>0b2</td><td>0b1</td><td>addr0</td></tr>
<tr><th>Pipe1</th><td>b4</td><td>b3</td><td>b2</td><td>b1</td><td>addr1</td></tr>
<tr><th>Pipe2</th><td>b4</td><td>b3</td><td>b2</td><td>b1</td><td>addr2</td></tr>
<tr><th>Pipe3</th><td>b4</td><td>b3</td><td>b2</td><td>b1</td><td>addr3</td></tr>
<tr><th>Pipe4</th><td>b4</td><td>b3</td><td>b2</td><td>b1</td><td>addr4</td></tr>
<tr><th>Pipe5</th><td>b4</td><td>b3</td><td>b2</td><td>b1</td><td>addr5</td></tr>
</table>

As a special requirement, addr0-addr5 must be different.

Pipe0 will be used as a general transmit permission request line with
`addr0=0x00`, and every further pipe gets dynamically assigned to a sensors,
using the sensors ID as addrX.

Only 3 byte adresses will be used, so the sent packages can be shorter.
Therefore, byte2 and byte1 are the unique system id, byte3 and byte4 are unused

## Auto-Ack and Payload

In my reading of the datasheet, Auto-Ack always takes the payload in the TX
FIFO of PRX and sends it to the PTX. As there can be only 3 payloads pending,
there is no benifit in using more than 3 data pipes and therefore 2 sensor
connections at once.

Pipe0 has Auto-Ack disabled, so that the master has time to put the specific
reply packet in the TX buffer. 

A communication process will look like the following:

1. PTX sends out communication request packet on Pipe0
    * free pipe: 
        1. set up pipeX with sensors id as addrX
        1. put ack packet into tx fifo for pipeX
        1. send out ack with sensor id as payload
    * no free pipe:
        1. ack with payload 0x00
1. PTX sends out transfer config/informational packet on pipeX
    * master auto acks with informational data packet
1. PTX sends out one packet for every reading
    * master auto acks

## RF Packets, rev0

### Pipe0 request

    payload[0] = sensor_id
    payload[1], lower nibble = packet design revision = 0x0
    payload[1], higher nibble = desired action
        bit4    data
        bit5    config
        bit6    unused
        bit7    new_sensor (sensor_id must be 0 here!)
        exactly one bit must be set

### Pipe0 ACK

    payload[0] = sensor_id (set to assigned sensor_id in case of config)
    payload[1], lower nibble = packet design revision (if smaller than
        requested, this is max. revision of master and comm is terminated)
    payload[1], higher nibble = repeat and confirm desired action
        bit4    ack data
        bit5    ack config
        bit6    unused
        bit7    ack new_sensor (set sensor_id!)
        no bit set means no free transmit pipe, try again later

### Data
#### PipeX first packet

    payload[0] = battery reading
        0x00        => empty with comparator
        0xFF        => not empty with comparator
        0x01-0xFE   => voltage in .02 volt steps (up to 5.1V)
    payload[1], lower nibble = number of readings to transmit (up to 16)
    payload[1], higher nibble = needs update

#### PipeX ACK

    payload[0] = transmit interval in 30s steps
    payload[1], lower nibble = outdoor brightness (adjust leds brightness)
    payload[1], higher nibble = unused

#### PipeX reading packets

    payload[0], xxDDPPPP
        xx      unused
        PPPP    probe id
        DD      bytes data to be recieved
    payload[1…4] = one to four probe data bytes
    
### Config

* [ ] write config protocol

## Thoughts
Up to 16 probes per sensor should be enough, else we need a sensor talking to
master as more than 1 sensor. 

Initially we need to brief the master about the known probes, especially their
data format and unit used. This should be splitted into two seperate processes,
a pairing of the sensor with the master (once) and probe configuration (every
time the probes change/new ones are added).

[0]: http://www.nordicsemi.com/kor/nordic/download_resource/8765/2/8827113 "nRF24L01+"

