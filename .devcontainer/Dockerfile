FROM ubuntu:22.04

# Set the correct locale so that non-ASCII characters can be processed correctly (https://stackoverflow.com/questions/28405902/how-to-set-the-locale-inside-a-ubuntu-docker-container/38553499#38553499)
# And set correct timezone (https://stackoverflow.com/questions/40234847/docker-timezone-in-ubuntu-16-04-image)
RUN apt update && apt install -y \
       locales \
       tzdata \
 && sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen \
 && dpkg-reconfigure --frontend=noninteractive locales \
 && update-locale LANG=en_US.UTF-8 \
 && ln -fs /usr/share/zoneinfo/Europe/Berlin /etc/localtime && dpkg-reconfigure -f noninteractive tzdata

ENV LANG en_US.UTF-8

# Basic tools
RUN apt update && apt install -y \
     build-essential \
     cmake \
     curl \
     gdb \
     git \
     inotify-tools \
     python3 python3-pip python-is-python3 \
     rsync \
     tree \
     vim \
     wget \
 && pip install numpy blosc2

# Install benchmarking tool
RUN wget https://github.com/sharkdp/hyperfine/releases/download/v1.16.1/hyperfine_1.16.1_amd64.deb \
 && dpkg -i hyperfine_1.16.1_amd64.deb \
 && rm hyperfine_1.16.1_amd64.deb

# Install Blosc2
RUN git clone https://github.com/Blosc/c-blosc2 \
 && cd c-blosc2 \
    && mkdir build \
    && cd build \
    && cmake -DCMAKE_INSTALL_PREFIX=/usr .. \
    && cmake --build . \
    && ctest \
    && cmake --build . --target install

WORKDIR /workspaces/blosc_mmap
