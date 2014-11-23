void setup()
{
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
}

int data = 0;

void loop()
{
	shiftOut(5,4,LSBFIRST,data);
	data++;
	if(data>=64)
	{
	 data=0;
	}
	delay(500);
}
