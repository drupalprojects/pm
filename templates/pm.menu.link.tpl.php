<dt class="pmcomponent">
  <?php if ($icon): ?>
    <?php echo $icon; ?>
  <?php endif ?>
  <a href="<?php echo $path; ?>"><?php echo $title; ?></a>
  <?php if ($extra_link): ?>
    <a href="<?php echo $extra_link; ?>" title="Add">
      <?php if ($extra_link_icon): ?>
        <?php echo $extra_link_icon; ?>
      <?php endif ?>
    </a>
  <?php endif ?>
</dt>