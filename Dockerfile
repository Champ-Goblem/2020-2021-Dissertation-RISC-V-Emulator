FROM gcc:10.2.0 as build

COPY . /build/

WORKDIR /build

RUN make

ENTRYPOINT [ "/build/obj/build/RISC-Emu" ]