# Realtime Video Chat App with Facial Recognition

This app utilizes Web Assembly and Elixir to provide a video chat app.

The facial recognition software uses a combination of OpenCV, DLib, and some
custom C++ code all compiled into web assembly.  The front end utilizes
Javascript's new found love for primitive data types to pass pointers to and
from native machine code to modify images.

The backend is just a pretty generic Elixir/OTP setup with Phoenix handling
some api functionality.
