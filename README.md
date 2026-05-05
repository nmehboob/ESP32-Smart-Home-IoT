# ESP32 Smart Home IoT System

## Overview

This project is a smart home automation system built using ESP32. It allows users to control multiple devices through a web interface and physical push buttons. The system provides real-time updates without page refresh.

## Features

* Control multiple devices via web browser
* Real-time updates using Server-Sent Events
* Physical button control
* Reset all devices functionality
* Scalable design using device array structure

## Hardware Used

* ESP32 microcontroller
* LEDs or Relay modules
* Push buttons
* Resistors

## Software & Technologies

* C++
* ESPAsyncWebServer
* WiFi (HTTP)
* Server-Sent Events

## How It Works

The ESP32 connects to WiFi and hosts a web server. A web page is served to the user, allowing device control through buttons. When a button is pressed (either on the webpage or physically), the ESP32 toggles the device state and sends real-time updates to the browser using Server-Sent Events.

## Project Structure

* src/main.ino → Main program code
* README.md → Project documentation




Mahboob Nawaz
