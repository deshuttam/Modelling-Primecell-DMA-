# Modelling Primecell DMA using SystemC

By Uttam Deshpande, Akshay Bhosale.

Hardware behaviour is the core of any electronic system and it is essential to know the hardware behaviour before mass producing the hardwares. Hence, to test the design we require a software model also known as virtual prototype, which can be used for testing before its production. Here we are using SystemC to model our peripheral. SystemC is basically a modelling library. It is a set of C++ classes and macros which enables a designer to simulate hardware behaviour. By creating models of the hardware, software development can be started early. This reduces the time to market of the product. The primary use of SystemC is high level systems modelling of hardware. One such hardware/peripheral example is the prime cell DMA controller provided by ARM limited. 
* The objective of the project is to model this peripheral using SystemC library and test its functionality using a test bench.

## Introduction
System Modelling is the process of developing abstract models of a system, with each model presenting a different view or perspective of that system. Virtual Prototyping (VP), is a software-based engineering discipline that involves modelling a system, simulating and visualising its behaviour below real world operational conditions, and purification its vogue through an reiterative methodology. VP is increasingly used as a substitute for quick prototyping. VP does not manufacture a entity for testing and analysis but, as it carries out task on a mere laptop. Virtual prototyping results in faster time-to-market through earlier and faster computer code package development and improved communication throughout the availability chain. They allow software package engineers to begin out development months before the hardware vogue is complete, enabling full system bring-up to occur within days of semiconductor accessibility. Virtual prototypes square measure fast, whole sensible computer code package models of complete systems that execute unrestricted production code and provide distinctive efficiency.
The first and foremost thing to do when working with any peripheral, be it system modelling or driver development, is to read the datasheet. Technical data sheets are one of the key documents of the products and present your product properties to customers. This documents includes information on key properties of the product like technical specifications, application or using methods, etc. Some excerpts from the datasheet of prime cell DMA controller have been used wherever necessary for the purpose of explanation.
![image](https://user-images.githubusercontent.com/107185323/198981358-ecd1509e-4fac-40ab-8859-db847445f056.png)

In SystemC, this method can be implemented using a simple bus interface which supports reads and writes. This is done in bus_if.h.Now that the entire project is divided and made modular, it can be put together.

![image](https://user-images.githubusercontent.com/107185323/198981523-a5de214d-003b-49cb-860e-ee60789b76c6.png)


## Test Cases
Case 1: - When source width is equal to destination width.
Steps: -
1) START
2) Check which channel needs to be served. If no channel is active go to step (9).
3) Check if the DMA controller is enabled. If not enabled go to step (10).
4) If transfer size = 0, set transmit complete interrupt, provided it is not masked out and go to step (2) else go to step (5)
5) If master port 1 is selected, read data from source using that port. If master port 2 is selected read data from source using that port.
6) If master port 1 is selected, write data to destination using that port. If master port 2 is selected write data to destination using that port.
7) In case any error occurs, set error interrupt provided it is not masked out and go to step (8) else go to step (9).
8) Decrement transfer size. Go to step (4)
9) END


Case 2: - When Source width is less than destination width.
Steps: -
1) START
2) Check which channel needs to be served. If no channel is active go to step (9).
3) Check if the DMA controller is enabled. If not enabled, go to step (9).
4) If transfer size = 0, set transmit complete interrrupt, provided it is not masked out and go to step (2) else go to step (5).
5) If master port 1 is selected, read data from source using that port. If master port 2 is selected read data from source using that port. Repeat this step till sufficient data is read to fill one destination register. For example, if the source width is 16 bits and destination width is 32 bits, read data from two 16-bit source registers and combine them as one.
6) If master port 1 is selected, write the combined data to destination using that port. If master port 2 is selected write the combined data to destination using that port. This constitutes one transfer.
7) In case any error occurs, set error interrupt provided it is not masked out and go to step (9) else go to step (8).
8) Decrement transfer size. Go to step (iv)
9) END


Case 3: - When Source width is greater than destination width.
Steps: -
1) START
2) Check which channel needs to be served. If no channel is active go to step (9).
3) Check if the DMA controller is enabled. If not enabled, go to step (9).
4) If transfer size = 0, set transmit complete interrupt, provided it is not masked out 
and go to step (2) else go to step (5).
5) If master port 1 is selected, read data from source using that port. If master port 2 is selected read data from source using that port.
6) If master port 1 is selected, write data to destination using that port. If master port 2 is selected write data to destination using that port. Repeat this step till all the data read from source has been transferred to multiple data registers. For example, if the source width is 32 bits and destination width is 16 bits, out of the 32 bit data, load the first 16 bit data into one destination register and load the next 16 bit data into another destination register. This constitutes one transfer.
7) In case any error occurs, set error interrupt provided it is not masked out and go to step (9) else go to step (8).
8) Decrement transfer size. Go to step (4)
9) END.

## Results 
Entire development was done on Ubuntu 16.04 operating system using SystemC library. 
Testing was done on the same operating system.
5.1.1 Results when only channel 0 is enabled Different cases considered :-
1) Destination Width :- 32 bits
Source Width :- 32 bits
Number of transfers :- 1
Increment source address after every transfer :- Yes
Increment destination address after every transfer :- Yes.
Master port 1 connected to source
Master port 1 connected to destination
![image](https://user-images.githubusercontent.com/107185323/198982515-01588212-91b9-4f92-b43c-e4f9aa54d1a2.png)


## The repository includes
1. DMA Code in C++
2. Test Bench Code in C++
3. Header Files.


## Usage
Working with Direct Memory Access (DMA) controller without the actual hardware is possible by System Modelling approach. During the synthesis of any hardware design this method can play a major role as software can be developed even without the actual hardware, thereby reducing the time to market. The DMA controller model has been designed for both the channels and it emulates the behaviour of the actual hardware. 

