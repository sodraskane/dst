<?php  
class Delegate extends \Illuminate\Database\Eloquent\Model {  
    protected $primaryKey = 'id';
    protected $table = 'delegates';
    protected $fillable = ['*'];
    public $timestamps = false;
}
