/**************************/
/*   B5WC_SampleProject   */
/**************************/
/* includes */
#include <Wire.h>

/* defines */
#define B5WC_ADDR   0x40  // B5WC I2C client address at 7bit expression
#define AVERAGE_NUM 20    // Average number of times set value, range 1～50[times]
#define READ_CYCLE  200   // Data readout cycle[ms], Recommended 1 ms or more

/** <!-- setup {{{1 -->
 * 1. initialize a Serial port for output.
 * 2. initialize an I2C peripheral.
 * 3. setup sensor settings.
 */
void setup() {
    byte send_data[2];
    
    /* 1. initialize a Serial port for output. */
    Serial.begin( 115200 );
    Serial.println( "[B5WC Sample project Start]" );
    Serial.println( "R,G,B Voltage[V], R,G,B Ratio[%]" );
    Serial.println( "--------------------------------" );

    /* 2. initialize an I2C peripheral. */
    Wire.begin();  // i2c master
    
    delay(100);

    /* 3. setup sensor settings. */
    send_data[0] = 0x01;
    send_data[1] = AVERAGE_NUM;
    Wire.beginTransmission( B5WC_ADDR );
    Wire.write( &send_data[0], (int)2 );
    Wire.endTransmission( true );   
}

/** <!-- loop - Color sensor {{{1 -->
 * 1. read and convert sensor.
 * 2. output results, format is: R,G,B Voltage[V], R,G,B Ratio[%]
 */
void loop() {
    byte receive_data[7];
    byte chksm;
    double R_vol, G_vol, B_vol;
    double R_rat, G_rat, B_rat;
    
    /* Data readout */
    Wire.beginTransmission( B5WC_ADDR );
    Wire.write((byte)0x02);
    Wire.endTransmission( false );
    Wire.requestFrom( B5WC_ADDR, 7, true );
    receive_data[0] = Wire.read();
    receive_data[1] = Wire.read();
    receive_data[2] = Wire.read();
    receive_data[3] = Wire.read();
    receive_data[4] = Wire.read();
    receive_data[5] = Wire.read();
    receive_data[6] = Wire.read();

    /* Check the checksum */
    chksm = 0xFF ^ receive_data[0] ^ receive_data[1] ^ receive_data[2] ^ receive_data[3] ^ receive_data[4] ^ receive_data[5];
    if ( receive_data[6] == chksm )
    {
        /* Data conversion */
        R_vol = (receive_data[1] << 8) | receive_data[0];
        G_vol = (receive_data[3] << 8) | receive_data[2];
        B_vol = (receive_data[5] << 8) | receive_data[4];

        R_vol = R_vol * 3.3 / 1024;
        G_vol = G_vol * 3.3 / 1024;
        B_vol = B_vol * 3.3 / 1024;

        if ( ( R_vol == 0 ) && ( G_vol == 0 ) && ( B_vol == 0 ) )
        {
            R_rat = 100;
            G_rat = 100;
            B_rat = 100;
        }
        else if ( ( R_vol >= G_vol ) && ( R_vol >= B_vol) )
        {
            R_rat = 100;
            G_rat = (G_vol / R_vol) * 100;
            B_rat = (B_vol / R_vol) * 100;
        }
        else if ( ( G_vol >= R_vol ) && ( G_vol >= B_vol) )
        {
            G_rat = 100;
            R_rat = (R_vol / G_vol) * 100;
            B_rat = (B_vol / G_vol) * 100;
        }
        else
        {
            B_rat = 100;
            R_rat = (R_vol / B_vol) * 100;
            G_rat = (G_vol / B_vol) * 100;
        }
        
        /* Output results */
        Serial.print( R_vol, 3 );
        Serial.print(",");
        Serial.print( G_vol, 3 );
        Serial.print(",");
        Serial.print( B_vol, 3 );
        Serial.print(",");
        Serial.print( R_rat, 1 );
        Serial.print(",");
        Serial.print( G_rat, 1 );
        Serial.print(",");
        Serial.println( B_rat, 1 );
    }

    delay( READ_CYCLE );
}
