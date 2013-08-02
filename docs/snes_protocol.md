SNES Controller Protocol
========================

*Based on a [this](http://tk-421projects.blogspot.com/2012/12/snes-controller-protocol.html) wonderful blog post.*

The SNES controller protocol is based on two parallel to serial shift registers. The host accesses the information by "Latching" the controller and receiving them by measuring the high and lows on the Data line.

Controller Wiring
-----------------

The controller plug has 7 pins, numbered 1 through 7, starting at the square end:

<table>
	<tr>
		<th>Pin</th>
		<th>Description</th>
		<th>Wire Color</th>
	</tr>
	<tr>
		<td>1</td>
		<td>+5v</td>
		<td>White</td>
	</tr>
	<tr>
		<td>2</td>
		<td>Data Clock</td>
		<td>Yellow</td>
	</tr>
	<tr>
		<td>3</td>
		<td>Data Latch</td>
		<td>Orange</td>
	</tr>
	<tr>
		<td>4</td>
		<td>Data</td>
		<td>Red</td>
	</tr>
	<tr>
		<td>5</td>
		<td>Unused</td>
		<td>-</td>
	</tr>
	<tr>
		<td>6</td>
		<td>Unused</td>
		<td>-</td>
	</tr>
	<tr>
		<td>7</td>
		<td>Gnd</td>
		<td>Brown</td>
	</tr>
</table>

*Note: The colors listed above are for first party controllers. Third party controllers may not have the same wiring colors.*

Controller Interfacing
----------------------

The protocol uses a latch and clock pin setup to trigger subsequent reads of button states through the data pin. The controller is *active low*, meaning buttons are considered pressed when the data pin is read LOW. Once the latch has been completed the first of 16 reads is available immediately. Pulsing the clock pin causes the controller to read subsequent buttons. The last 4 of 16 reads are not used.

In the setup method:

1. Set pinMode for latch pin to OUTPUT
2. Set pinMode for clock pin to OUTPUT
3. Set pinMode for data pin to INPUT
4. Set latch pin HIGH
5. Set clock pin HIGH

In the loop method:

1. Set latch pin LOW
2. Set clock pin LOW
3. Set latch pin HIGH for ~12us (triggers the controller to start data read)
4. Set latch pin LOW
5. Delay for ~6us


