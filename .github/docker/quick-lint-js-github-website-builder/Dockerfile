FROM ubuntu:20.04

RUN echo 'Acquire::http::Proxy "http://192.168.0.21:3142";' > /etc/apt/apt.conf.d/00aptproxy

RUN \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      curl \
      gnupg \
      software-properties-common && \
    curl 'https://deb.nodesource.com/gpgkey/nodesource.gpg.key' | apt-key add - && \
    apt-add-repository 'deb https://deb.nodesource.com/node_14.x focal main' && \
    sed -n -e 's/^deb /deb-src /p' /etc/apt/sources.list >/etc/apt/sources.list.d/quick-lint-js-sources.list 

RUN \
    apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        cmake \
        gcc \
        ninja-build \
        python3.7 \
        python3-pip \
        apache2 \ 
        rsync \ 
        zip \ 
        unzip \
        tar \
        xz-utils \
        nodejs \
        git

RUN \ 
    bash -v && \
    git clone https://github.com/emscripten-core/emsdk.git && \
    cd emsdk && \
    ./emsdk install latest 
    # ./emsdk activate latest

RUN npm install -g yarn 

RUN pip install bs4 requests 

RUN \
    a2enmod deflate headers && \
    sed -i -e 's/AllowOverride\ None/AllowOverride\ FileInfo/' /etc/apache2/apache2.conf

COPY entrypoint.sh /entrypoint.sh
RUN chmod a+x /entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]
