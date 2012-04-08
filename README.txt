UBERCART : DELIVERY README

This module is used to allow for the dynamic addition of arbitrary fees to any
order currently in progress.  This module is great for web sites who offer a
shipping service requiring additional fees or up-sells that may not be
represented as actual products.

INSTALLATION:

Install as you would any other contrib module.

CONFIGURATION:

This module can be configured by navigating to:
Store Administration > Configuration > Delivery fees overview

This section allows you to configure the fieldset title and description which
appear on the cart/checkout page. It also lists the delivery options that have
been set up.

To set up delivery options, go to:
Store Administration > Configuration > Delivery fees overview > Add delivery option

Delivery options consist of the following:

Fee Title: the actual fee item title.
Fee Description: helper text for the end user.
Enabled: whether or not this option is enabled.
Fee Options: a pipe-delimited list of <price>|<fee option> values.
Fee Weight: the display order for this fee option.
Widget type: which type of form element to use to display the option.

Fee Options examples:
It is generally useful to add a $0.00 option if the fee isn't required:
0.00|No stairs
5.00|1 flight
10.00|2 flights
20.00|3 flights

Once configured, navigate to the panes section of the "Checkout Settings"
within Ubercart: admin/store/settings/checkout/edit/panes
