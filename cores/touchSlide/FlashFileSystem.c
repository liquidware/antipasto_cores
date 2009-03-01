//*******************************************************************************
//*	Flsh Memory File System
//*		by Mark Sproul
//*		this source code is part of the liquid ware drivers for the 
//*		TouchShield and TouchSlide
//*		it is public domain under the the GPL as is the rest of the Arduino family
//*
//*	These routines handle the file system for the Flash Memory (16 megabit, 2 megaBYTE)
//*	on board the screens.
//*******************************************************************************
//*	Jan 10,	2009	<MLS> Started mergering stealth/slide versions of imaage_interface.c
//*	Jan 11,	2009	<MLS> Mostly done with merge
//*	Jan 14,	2009	<MLS> Added status messages via debug rect
//*	Jan 15,	2009	<MLS> Discussed File System with Matt, decided to do entirely new system
//*	Jan 16,	2009	<MLS> Got my own C program on MacOSX to talk to the USB port and to this code
//*	Jan 17,	2009	<MLS> Strated seperate file for new file system
//*******************************************************************************

#warning this file has been replaced with FlashFileSystem.cpp