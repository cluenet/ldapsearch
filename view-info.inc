<!-- One result, info view -->
<?php if (isset($Photo)): ?>
<div class="photobox" style="float: right;">
<img src="data:image/jpeg;base64,<?php echo base64_encode($Photo); ?>" />
</div>
<?php endif; ?>

<?php include "view-account.inc"; ?>

<?php include "view-person.inc"; ?>

<?php include "view-contact.inc"; ?>

<?php include "view-access.inc"; ?>
